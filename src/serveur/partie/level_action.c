
#include "level_action.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "level_update.h"
#include "level.h"

#include "utils.h"
#include "constants.h"

/**
 * @brief Routine pour le threads de respawn d'un objet
 * 
 * @param arg Tableau avec une sharedMemory et un Objet
 * @return void* Non utilisé
 */
void *respawn_routine(void *arg) {
	// Recuperer les arguments
	void **args = (void**)arg;
	threadsSharedMemory *sharedMemory = (threadsSharedMemory*)args[0];
	Objet *obj = (Objet*)args[1];
    // Free the arguments
	free(args);
	// Wait 5 seconds
	sleep(5);
	// Lock the mutex
	pthread_mutex_lock(&sharedMemory->mutex);
	// Respawn the object
	obj->isActive = true;
	// Signal condition variable
	pthread_cond_broadcast(&sharedMemory->update_cond);
	pthread_mutex_unlock(&sharedMemory->mutex);
	return NULL;
}

/**
 * @brief Fonction qui lance un thread pour respawn un objet
 * 
 * @param sharedMemory La sharedMemory
 * @param obj Objet à respawn
 */
void launch_respawn_routine(threadsSharedMemory *sharedMemory, Objet *obj) {
	// Create the thread
	pthread_t thread;
	// Create the arguments
	void **args = malloc(2 * sizeof(void*));
	args[0] = sharedMemory;
	args[1] = obj;
	// Create the thread
	if (pthread_create(&thread, NULL, respawn_routine, args) == -1) {
        perror("pthread_create");
        logs(L_INFO, "pthread_create failed %s", strerror(errno));
    }
}

/**
 * @brief Routine pour le threads de bombe.
 * 
 * @param arg Tableau avec une sharedMemory et un Objet
 * @return void* Non utilisé
 */
void *bomb_routine(void *arg) {
    // Recuperer les arguments
    void **args = (void**)arg;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)args[0];
    Objet *obj = (Objet*)args[1];
    Level *lvl = (Level*)args[2];
    // Free the arguments
    free(args);
    // Wait 5 seconds
    sleep(5);
    // Lock the mutex
    pthread_mutex_lock(&sharedMemory->mutex);
    
    // Prendre les joueurs et mobs dans la zone d'explosion (5blocks)
    Liste mobs = mobsInHitBox(sharedMemory, lvl, obj->x-2, obj->y, 5, 5);
    // Pour chaque mob dans la zone d'explosion
    EltListe *e = mobs.tete;
    while (e != NULL) {
        // Recuperer l'objet
        MobThreadsArgs *mobArgs = (MobThreadsArgs*)e->elmt;
        Objet *mob = mobArgs->mob;

        // Vérifier la distance
        int distance = abs(obj->x - mob->x) + abs(obj->y - mob->y);
        if (distance > 5) {
            // Next element
            e = e->suivant;
            continue;
        }

        // On freeze le mob
        mobArgs->isFreeze = true;

        printf("Mob %d vient de se prendre une bombe !\n", mob->id);
        logs(L_DEBUG, "Mob %d vient de se prendre une bombe !", mob->id);

        // Next element
        e = e->suivant;
    }
    // Free the list
    liste_free(&mobs, false);
    // Pour chaque joueur dans la zone d'explosion
    Liste players = playersInHitBox(sharedMemory, lvl, obj->x-2, obj->y, 5, 5);
    printf("Nombre de joueurs dans la zone d'explosion: %d (x: %d, y: %d)\n", players.taille, obj->x, obj->y);
    e = players.tete;
    while (e != NULL) {
        // Recuperer le joueur
        Player *player = (Player*)e->elmt;
        // Vérifier la distance
        int distance = abs(obj->x - player->obj->x) + abs(obj->y - player->obj->y);
        if (distance > 5) {
            // Next element
            e = e->suivant;
            continue;
        }


        printf("Joueur %s vient de se prendre une bombe !\n", player->name);
        logs(L_DEBUG, "Joueur %s vient de se prendre une bombe !", player->name);

        if (player->isFreeze) {
            // On retire une vie au joueur car juste freeze mais pas invincible
            if (player->life > 1) {
                player->life--;
            } else {
                // On tue le joueur
                player->life--;
                player->isAlive = false;
                // TODO Faire quelque quand le joueur est mort
            }
        } else {
            // On freeze le joueur
            player->isFreeze = true;
            // On retire une vie au joueur
            if (player->life > 1) {
                player->life--;
                // On lance un thread pour le défreeze
                launch_unfreeze_player_routine(sharedMemory, player);
                // Envoie message au client
                privateMessage(sharedMemory, player->numPlayer, "Vous êtes gelé !", RED_COLOR, 1);
            } else {
                // On tue le joueur
                player->life--;
                player->isAlive = false;
                // TODO Faire quelque quand le joueur est mort
            }
        }

        // Next element
        e = e->suivant;
    }
    // Free the list
    liste_free(&players, false);

    // Supprimer la bombe
    levelSupprimerObjet(lvl, obj);

    // Signal condition variable
    pthread_cond_broadcast(&sharedMemory->update_cond);
    pthread_mutex_unlock(&sharedMemory->mutex);
    return NULL;
}

void *unfreeze_player_routine(void *arg) {
    // Recuperer les arguments
    void **args = (void**)arg;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)args[0];
    Player *player = (Player*)args[1];
    // Free the arguments
    free(args);
    // Wait 5 seconds
    sleep(5);
    // Lock the mutex
    pthread_mutex_lock(&sharedMemory->mutex);
    // Unfreeze the player
    player->isFreeze = false;
    // Envoie message au client
    privateMessage(sharedMemory, player->numPlayer, "Vous n'êtes plus gelé !", GREEN_COLOR, 1);
    // Signal condition variable
    pthread_cond_broadcast(&sharedMemory->update_cond);
    pthread_mutex_unlock(&sharedMemory->mutex);
    return NULL;
}

void launch_unfreeze_player_routine(threadsSharedMemory *sharedMemory, Player *player) {
    // Create the thread
    pthread_t thread;
    // Create the arguments
    void **args = malloc(3 * sizeof(void*));
    args[0] = sharedMemory;
    args[1] = player;
    // Create the thread
    if (pthread_create(&thread, NULL, unfreeze_player_routine, args) == -1) {
        perror("pthread_create");
        logs(L_INFO, "pthread_create failed %s", strerror(errno));
    }
}

/**
 * @brief Fonction qui lance un thread pour une bombe
 * 
 * @param sharedMemory 
 * @param obj 
 */
void launch_bomb_routine(threadsSharedMemory *sharedMemory, Objet *obj, Level *lvl) {
    // Create the thread
    pthread_t thread;
    // Create the arguments
    void **args = malloc(3 * sizeof(void*));
    args[0] = sharedMemory;
    args[1] = obj;
    args[2] = lvl;
    // Create the thread
    if (pthread_create(&thread, NULL, bomb_routine, args) != 0) {
        perror("pthread_create");
        logs(L_INFO, "pthread_create failed %s", strerror(errno));
    }
}

/**
 * @brief Méthode qui verifie les différents mouvement du joueurs et action possible ou non.
 * 
 * @param player Le joueur qui bouge
 * @param level Le niveau dans le quelle le joueur bouge
 * @param newX La nouvelle position X du joueur
 * @param newY La nouvelle position Y du joueur
 */
void player_action(Player *player, Level *level, short newX, short newY, threadsSharedMemory *sharedMemory) {
    bool canMoveX = true;
    bool canMoveY = true;
    bool hasLadder = false;
    bool hasBlockUnderFeet = false;

    // Check if the player is on a ladder
    Liste oldCollision = objectInHitBox(level, player->obj->x, player->obj->y, 3, 4);
    // Parcourir la liste
    if (oldCollision.tete != NULL) {
        EltListe* elt = oldCollision.tete;
        while (elt != NULL) {
            Objet* obj = (Objet*)elt->elmt;
            // Check if the player is on a ladder perfectly centered
            if (obj->type == LADDER_ID && obj->x == player->obj->x && obj->y == player->obj->y) {
                hasLadder = true;
                break;
            }
            elt = elt->suivant;
        }
    }
    liste_free(&oldCollision,false);

    // Check if there is a collision in the new position
    Liste newCollision = objectInHitBox(level, newX, newY, 3, 4);
    // Parcourir la liste
    if (newCollision.tete != NULL) {
        EltListe* elt = newCollision.tete;
        while (elt != NULL) {
            Objet* obj = (Objet*)elt->elmt;
            if (!obj->isActive) {
                elt = elt->suivant;
                continue;
            }

            if (obj->type == BLOCK_ID || obj->type == TRAP_ID) {
                canMoveX = false;
                canMoveY = false;
            } else if (obj->type == GATE_ID) {
                switch (obj->gate.numgate) {
                    case 0:
                        if (!player->key1) {
                            // Empeche le joueur de passer
                            canMoveX = false;
                        }
                        break;
                    case 1:
                        if (!player->key2) {
                            // Empeche le joueur de passer
                            canMoveX = false;
                        }
                        break;
                    case 2:
                        if (!player->key3) {
                            // Empeche le joueur de passer
                            canMoveX = false;
                        }
                        break;
                    case 3:
                        if (!player->key4) {
                            // Empeche le joueur de passer
                            canMoveX = false;
                        }
                        break;
                }
            } else if (obj->type == LADDER_ID) {
                // Check if the player is on the top of the ladder perfectly centered
                if (obj->x == newX && obj->y == newY) {
                    hasLadder = true;
                }
            } else if (obj->type == KEY_ID) {
                switch (obj->key.numkey) {
                    case 0:
                        player->key1 = true;
                        break;
                    case 1:
                        player->key2 = true;
                        break;
                    case 2:
                        player->key3 = true;
                        break;
                    case 3:
                        player->key4 = true;
                        break;
                }
            } else if (obj->type == HEART_ID) {
                if (player->life < 5) {
                    player->life++;
                    obj->isActive = false;
					launch_respawn_routine(sharedMemory, obj);
                }
            } else if (obj->type == BOMB_ID) {
                if (player->nbBombs < 3) {
                    player->nbBombs++;
                    obj->isActive = false;
					launch_respawn_routine(sharedMemory, obj);
                }
            } else if (obj->type == EXIT_ID) {
				char msg[500];
				sprintf(msg, "Le joueur %s a gagné !", player->name);
				broadcastMessage(sharedMemory, msg, GREEN_COLOR, 1);
			} else if (obj->type == DOOR_ID) {
				DoorLink doorLink = sharedMemory->doors[obj->door.numdoor];
				if (doorLink.isLinked) {
					broadcastMessage(sharedMemory, "Appuyer sur 'Entrer' pour utiliser la porte !", YELLOW_COLOR, 1);

				} else {
					broadcastMessage(sharedMemory, "Cette porte ne semble pas fonctionner...", PURPLE_COLOR, 1);
				}
			}
            elt = elt->suivant;
        }
    }
	liste_free(&newCollision, false);

    // Check if there is a block under the new position
    Liste underPlr = objectInHitBox(level, newX, newY+1, 3, 1);
    // Parcourir la liste
    if (underPlr.tete != NULL) {
        EltListe* elt = underPlr.tete;
        while (elt != NULL) {
            Objet* obj = (Objet*)elt->elmt;
            if (obj->type == BLOCK_ID || obj->type == TRAP_ID || obj->type == LADDER_ID) {
                hasBlockUnderFeet = true;
                break;
            }
            elt = elt->suivant;
        }
    }
    liste_free(&underPlr, false);

    // Check if the player can move
    if (canMoveX && hasBlockUnderFeet) {
        player->obj->x = newX;
    }
    if (canMoveY && hasLadder) {
        player->obj->y = newY;
    }
}

/**
 * @brief Change le joueur de niveau.
 * 
 * @param sharedMemory La mémoire partagée.
 * @param level Son nouveau niveau.
 * @param x Le x de sa nouvelle position.
 * @param y Le y de sa nouvelle position.
 */
void changePlayerOfLevel(threadsSharedMemory *sharedMemory, Player *player, Level *oldLevel, Level *newLevel, short x, short y) {
	// On déplace le joueur de son ancien niveau à son nouveau niveau
	levelChangeLevelObjet(oldLevel, newLevel, player->obj, x, y);
	// On change le niveau du joueur
	player->level = newLevel->levelNumber + 1;
}

/**
 * @brief Construit la liste des DoorLink linkant les portes de même numéro entre elles.
 * 
 * @param doors La liste des portes (couple porte et niveau).
 * @return Liste La liste des DoorLink.
 */
DoorLink* create_doorlink(Liste *doors) {
	DoorLink* linkDoor = malloc(sizeof(DoorLink) * MAX_DOORS_NUMBER);
	int isDoorLinked[MAX_DOORS_NUMBER];
	memset(linkDoor, 0, sizeof(DoorLink) * MAX_DOORS_NUMBER);
	memset(isDoorLinked, 0, sizeof(isDoorLinked));

	for (int i = 0; i < doors->taille; i++) {
		Door *door = (Door*)liste_get(doors, i);
		// Si la porte n'a pas encore de DoorLink
		if (isDoorLinked[door->door->door.numdoor] == 1) {
			// Ajouter la porte au DoorLink
			linkDoor[door->door->door.numdoor].door2 = *door;
			linkDoor[door->door->door.numdoor].isLinked = true;
		} else if (isDoorLinked[door->door->door.numdoor] == 0) {
			// Créer un nouveau DoorLink
			linkDoor[door->door->door.numdoor].door1 = *door;
			isDoorLinked[door->door->door.numdoor] = 1;
		}
	}

	return linkDoor;
}

