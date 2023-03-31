
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
	LevelMutex *levelMutex = (LevelMutex*)args[2];
	// Free the arguments
	free(args);
	// Wait 5 seconds
	sleep(5);
	// Lock the mutex
	pthread_mutex_lock(&levelMutex->mutex);
	// Respawn the object
	obj->isActive = true;
	// Signal condition variable
	pthread_cond_broadcast(&sharedMemory->update_cond);
	pthread_mutex_unlock(&levelMutex->mutex);
	return NULL;
}

/**
 * @brief Fonction qui lance un thread pour respawn un objet
 * 
 * @param sharedMemory La sharedMemory
 * @param obj Objet à respawn
 */
void launch_respawn_routine(threadsSharedMemory *sharedMemory, Objet *obj, LevelMutex *levelMutex) {
	// Create the thread
	pthread_t thread;
	// Create the arguments
	void **args = malloc(3 * sizeof(void*));
	args[0] = sharedMemory;
	args[1] = obj;
	args[2] = levelMutex;
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
	LevelMutex *levelMutex = (LevelMutex*)args[2];
	// Free the arguments
	free(args);
	// Wait 5 seconds
	sleep(5);
	// Lock the mutex
	pthread_mutex_lock(&levelMutex->mutex);
	
	// Prendre les joueurs et mobs dans la zone d'explosion (5blocks)
	pthread_mutex_lock(&sharedMemory->mutex);
	Liste mobs = mobsInHitBox(sharedMemory, &levelMutex->level, obj->x-2, obj->y, 5, 5);
	pthread_mutex_unlock(&sharedMemory->mutex);
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
	pthread_mutex_lock(&sharedMemory->mutex);
	Liste players = playersInHitBox(sharedMemory, &levelMutex->level, obj->x-2, obj->y, 5, 5);
	pthread_mutex_unlock(&sharedMemory->mutex);
	printf("Nombre de joueurs dans la zone d'explosion: %d (x: %d, y: %d)\n", players.taille, obj->x, obj->y);
	e = players.tete;
	while (e != NULL) {
		// Recuperer le joueur
		Player *player = (Player*)e->elmt;

		// Verifier si en vie
		if (!player->isAlive) {
			// Next element
			e = e->suivant;
			continue;
		}

		// Vérifier la distance
		int distance = abs(obj->x - player->obj->x) + abs(obj->y - player->obj->y);
		if (distance > 5) {
			// Next element
			e = e->suivant;
			continue;
		}

		// Logs
		printf("Joueur %s vient de se prendre une bombe !\n", player->name);
		logs(L_DEBUG, "Joueur %s vient de se prendre une bombe !", player->name);

		pthread_mutex_lock(&sharedMemory->mutex);
		if (player->isFreeze) {
			// On retire une vie au joueur car juste freeze mais pas invincible
			if (player->life > 1) {
				player->life--;
			} else {
				// On tue le joueur
				player->life--;
				death_player_routine(sharedMemory, player);
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
				death_player_routine(sharedMemory, player);
			}
		}
		pthread_mutex_unlock(&sharedMemory->mutex);

		// Next element
		e = e->suivant;
	}
	// Free the list
	liste_free(&players, false);

	// Supprimer la bombe
	levelSupprimerObjet(&levelMutex->level, obj);

	// Signal condition variable
	pthread_cond_broadcast(&sharedMemory->update_cond);
	pthread_mutex_unlock(&levelMutex->mutex);
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
void launch_bomb_routine(threadsSharedMemory *sharedMemory, Objet *obj, LevelMutex *levelMutex) {
	// Create the thread
	pthread_t thread;
	// Create the arguments
	void **args = malloc(3 * sizeof(void*));
	args[0] = sharedMemory;
	args[1] = obj;
	args[2] = levelMutex;
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
void player_action(Player *player, LevelMutex *levelMutex, short newX, short newY, threadsSharedMemory *sharedMemory) {
	bool canMoveX = true;
	bool canMoveY = true;
	bool hasLadder = false;
	bool hasBlockUnderFeet = false;

	// Check if the player is on a ladder
	Liste oldCollision = objectInHitBox(&levelMutex->level, player->obj->x, player->obj->y, 3, 4);
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
	Liste newCollision = objectInHitBox(&levelMutex->level, newX, newY, 3, 4);
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
			}
			else if (obj->type == GATE_ID) {
				// Empeche le joueur de passer si il n'a pas la clé
				canMoveX = !(0
					|| (obj->gate.numgate == 0 && !player->key1)
					|| (obj->gate.numgate == 1 && !player->key2)
					|| (obj->gate.numgate == 2 && !player->key3)
					|| (obj->gate.numgate == 3 && !player->key4)
				);

			}
			else if (obj->type == LADDER_ID) {
				// Check if the player is on the top of the ladder perfectly centered
				if (obj->x == newX && obj->y == newY) {
					hasLadder = true;
				}
			}
			else if (obj->type == KEY_ID) {
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
			}
			else if (obj->type == HEART_ID) {
				if (player->life < 5) {
					player->life++;
					obj->isActive = false;
					launch_respawn_routine(sharedMemory, obj, levelMutex);
				}
			}
			else if (obj->type == BOMB_ID) {
				if (player->nbBombs < 3) {
					player->nbBombs++;
					obj->isActive = false;
					launch_respawn_routine(sharedMemory, obj, levelMutex);
				}
			}
			else if (obj->type == EXIT_ID) {
				char msg[500];
				sprintf(msg, "Le joueur %s a gagné !", player->name);
				broadcastMessage(sharedMemory, msg, GREEN_COLOR, 1);
			}
			else if (obj->type == DOOR_ID) {
				DoorLink doorLink = sharedMemory->doors[obj->door.numdoor];
				if (doorLink.isLinked) {
					privateMessage(sharedMemory, player->numPlayer, "Appuyer sur 'Entrer' pour utiliser la porte !", YELLOW_COLOR, 1);
				} else {
					privateMessage(sharedMemory, player->numPlayer, "Cette porte ne semble pas fonctionner...", PURPLE_COLOR, 1);
				}
			}
			elt = elt->suivant;
		}
	}
	liste_free(&newCollision, false);

	// Check if there is a block under the new position
	if (player->obj->y == newY) {
		Liste underPlr = objectInHitBox(&levelMutex->level, newX, newY+1, 3, 1);
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
	}

	// Si pas de bloc sous les pieds on fait tomber le joueur
	if (!hasBlockUnderFeet && !hasLadder && player->obj->y == newY) {
		// On récupére tout les blocs sous les pieds du joueur
		Liste underPlr = objectInHitBox(&levelMutex->level, newX, MATRICE_LEVEL_Y-1, 3, MATRICE_LEVEL_Y-player->obj->y+1);
		// Parcourir la liste
		if (underPlr.taille > 0) {
			short highestY = 20;
			EltListe* elt = underPlr.tete;
			while (elt != NULL) {
				Objet* obj = (Objet*)elt->elmt;
				if (obj->type == BLOCK_ID || obj->type == TRAP_ID || obj->type == LADDER_ID || obj->type == GATE_ID) {
					if (obj->y < highestY) {
						highestY = obj->y;
					}
				}
				elt = elt->suivant;
			}

			printf("Le joueur %s tombe de %d en %d !\r ", player->name, player->obj->y, highestY);
			fflush(stdout);

			// On met à jour la position du joueur
			player->obj->x = newX;
			player->isFalling = true;

			// Call fall routine
			launch_fall_player_routine(sharedMemory, player, highestY);
		} else {
			// Sort de la map donc mort mais pas normal
			death_player_routine(sharedMemory, player);
		}
		liste_free(&underPlr, false);
	} else {
		// Check if the player can move
		if (canMoveX) {
			player->obj->x = newX;
		}
		if (canMoveY && hasLadder) {
			player->obj->y = newY;
		}
	}
}

void *fall_player_routine(void *arg) {
	threadsSharedMemory *sharedMemory = ((void**)arg)[0];
	Player *player = ((void**)arg)[1];
	short *highestYD = ((void**)arg)[2];
	short highestY = *highestYD;
	free(arg);
	free(highestYD);
	pthread_mutex_lock(&sharedMemory->mutex);

	short nbBlock = 0;

	// On fait tomber le joueur de 1 en 1 jusqu'à la hauteur du bloc le plus haut
	while (player->obj->y < highestY-1) {
		player->obj->y++;
		nbBlock++;
		// On envoie la mise à jour au client
		pthread_cond_broadcast(&sharedMemory->update_cond);
		pthread_mutex_unlock(&sharedMemory->mutex);
		usleep(100000);
		pthread_mutex_lock(&sharedMemory->mutex);
	}

	// Si sort de la map on le tue
	if (player->obj->y >= MATRICE_LEVEL_Y-1) {
		death_player_routine(sharedMemory, player);
	} else if (nbBlock > 2) { // Si on tombe de plus de 2 blocs on perd une vie
		// On retire 1 de vie au joueur
		if (player->life-1 > 0) {
			player->life--;
			// On envoie un message au client
			privateMessage(sharedMemory, player->numPlayer, "Vous avez perdu une vie en tombant !", RED_COLOR, 1);
		} else {
			// On tue le joueur
			player->life = 0;
			death_player_routine(sharedMemory, player);
		}
	}

	// Desactive falling
	player->isFalling = false;

	// Unlock mutex && cond
	pthread_mutex_unlock(&sharedMemory->mutex);
	pthread_cond_broadcast(&sharedMemory->update_cond);

	return NULL;
}

void launch_fall_player_routine(threadsSharedMemory *sharedMemory, Player *player, short highestY) {
	// On crée un thread pour faire tomber le joueur
	void* args = malloc(sizeof(void*)*3);
	((void**)args)[0] = sharedMemory;
	((void**)args)[1] = player;
	short *highestYA = malloc(sizeof(short));
	*highestYA = highestY;
	((void**)args)[2] = highestYA;
	pthread_t thread;
	if (pthread_create(&thread, NULL, fall_player_routine, (void*)args)) {
		perror("pthread_create");
		logs(L_INFO, "Erreur lors de la création du thread fall_player_routine");
		exit(EXIT_FAILURE);
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
void changePlayerOfLevel(threadsSharedMemory *sharedMemory, Player *player, LevelMutex *oldLevel, LevelMutex *newLevel, short x, short y) {
	// On lock les mutex des deux niveaux
	lockMutexAvoidDeadLock(&oldLevel->mutex, &newLevel->mutex);

	// On déplace le joueur de son ancien niveau à son nouveau niveau
	levelChangeLevelObjet(&oldLevel->level, &newLevel->level, player->obj, x, y);
	// On change le niveau du joueur
	player->level = newLevel->level.levelNumber + 1;

	// On débloque les mutex
	pthread_mutex_unlock(&oldLevel->mutex);
	pthread_mutex_unlock(&newLevel->mutex);
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

/**
 * @brief Manage player's death.
 * 
 * @param sharedMemory	Shared memory.
 * @param player		The player.
*/
void death_player_routine(threadsSharedMemory *sharedMemory, Player *player) {

	// On met le joueur en mort
	player->isAlive = false;
	player->life = 0;

	// On supprime le joueur de la map
	Level *level = liste_get(&sharedMemory->levels, player->level-1);
	levelSupprimerObjet(level, player->obj);

	// Set to NULL to avoid double free
	player->obj = NULL;

	// Envoie d'un message au joueur
	privateMessage(sharedMemory, player->numPlayer, "Vous êtes mort, 'ENTRER' pour recommencer, 'Q' pour quitter.", RED_COLOR, 0);

	// Logs
	logs(L_INFO, "PartieManager | partieProcessusManager | Le joueur %d est mort", player->numPlayer);
	printf("PartieManager | partieProcessusManager | Le joueur %d est mort \n", player->numPlayer);
}

/**
 * @brief Manage player's respawn.
 * 
 * @param sharedMemory	Shared memory.
 * @param player		The player.
*/
void respawn_player_routine(threadsSharedMemory *sharedMemory, Player *player) {

	// On met le joueur en vie
	player->life = 5;
	player->nbBombs = 0;
	player->isAlive = true;
	player->isFreeze = false;
	player->isInvincible = false;
	player->isFalling = false;
	player->key1 = false;
	player->key2 = false;
	player->key3 = false;
	player->key4 = false;
	player->level = sharedMemory->levelEnter+1;

	// On place le joueur sur la map au bon endroit
	Level *level = liste_get(&sharedMemory->levels, sharedMemory->levelEnter);
	player->obj = poserPlayer(level, sharedMemory->enterX, sharedMemory->enterY);
	if (player->obj == NULL) {
		logs(L_DEBUG, "PartieManager | partieProcessusManager | poserPlayer == NULL");
		printf("PartieManager | partieProcessusManager | Error while placing the player on the map\n");
		exit(EXIT_FAILURE);
	}
	player->obj->player.color = 1 + player->numPlayer % 7;
	player->obj->player.orientation = RIGHT_ORIENTATION;

	// On envoie un message au joueur
	privateMessage(sharedMemory, player->numPlayer, "Vous êtes de retour !", GREEN_COLOR, 1);
	privateMessage(sharedMemory, player->numPlayer, "Press 'Q' to quit...", RED_COLOR, 0);

	// Logs
	logs(L_INFO, "PartieManager | partieProcessusManager | Le joueur %d est de retour !", player->numPlayer);
	printf("PartieManager | partieProcessusManager | Le joueur %d est de retour ! \n", player->numPlayer);
}

