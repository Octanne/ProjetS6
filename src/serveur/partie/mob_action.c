
#include "mob_action.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "constants.h"
#include "level_update.h"
#include "partie_manager.h"
#include "level_action.h"

void *uninvincible_player_routine(void *arg) {

    // Recuperer les arguments
    void **args = (void**)arg;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)args[0];
    Player *player = (Player*)args[1];

    // Free the arguments
    free(args);

    // Wait 3 seconds
    sleep(3);

    // Lock the mutex
    pthread_mutex_lock(&sharedMemory->mutex);

    // Uninvincible the player
    player->isInvincible = false;

    // Envoie message au client
    privateMessage(sharedMemory, player->numPlayer, "Vous n'êtes plus invincible !", GREEN_COLOR, 1);

    // Unlock the mutex
    pthread_mutex_unlock(&sharedMemory->mutex);
    return NULL;
}

void launch_uninvincible_player_routine(threadsSharedMemory *sharedMemory, Player *player) {

    // Create the thread
    pthread_t thread;

    // Create the arguments
    void **args = malloc(3 * sizeof(void*));
    args[0] = sharedMemory;
    args[1] = player;

    // Create the thread
    if (pthread_create(&thread, NULL, uninvincible_player_routine, args) == -1) {
        perror("pthread_create");
        logs(L_INFO, "pthread_create failed %s", strerror(errno));
    }
}

void* probe_routine(void * args) {
    // Recuperer les arguments
    void **argsTable = (void**)args;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)argsTable[0];
    MobThreadsArgs *argsMob = (MobThreadsArgs*)argsTable[1];
    // Free the arguments
    free(args);

    logs(L_INFO, "Routine | Lancement routine probe %d", argsMob->mob->id);

    // Si jeux fini alors th_shared_memory.game_state = 2
    while (sharedMemory->game_state != 2) {
        // Lock the mutex
        pthread_mutex_lock(&sharedMemory->mutex);

        // Mise en pause si freeze et reveil sur condition
        if (argsMob->isFreeze) {
            printf("Probe %d is freeze\n", argsMob->mob->id);
            // On fait sleep le temps du freeze soit 5 secondes
            pthread_mutex_unlock(&sharedMemory->mutex);
            sleep(5);
            pthread_mutex_lock(&sharedMemory->mutex);
            argsMob->isFreeze = false;
            printf("Probe %d is unfreeze\n", argsMob->mob->id);
        }

        // Choix d'un random entre 0 et 3
        int random = rand() % 4;
        int newX = argsMob->mob->x;
        int newY = argsMob->mob->y;
        
        // Si le random est 0 alors on bouge vers le haut
        if (random == 0) {
            newY = argsMob->mob->y - 1;
        } else if (random == 1) {
            newY = argsMob->mob->y + 1;
        } else if (random == 2) {
            newX = argsMob->mob->x - 1;
        } else if (random == 3) {
            newX = argsMob->mob->x + 1;
        }

        // Check hitbox
        ObjetSize size = objet_getSize(argsMob->mob); // On augment la hitbox de 1 pour pouvoir voir les pièges aux pieds.
        Liste liste = objectInHitBox(&argsMob->levelMutex->level, newX, newY+1, size.xSize, size.ySize+1);
        // Si la liste est vide alors on peut bouger
        if (liste.tete != NULL) {
            bool canMove = true;
            // On parcoure la liste pour voir si il y a un joueur
            EltListe *element = liste.tete;
            while (element != NULL) {
                Objet *objet = (Objet*)element->elmt;
                if ((objet->y <= newY && (objet->type == BLOCK_ID || objet->type == GATE_ID)) 
                    || (objet->type == TRAP_ID)) {
                    canMove = false;
                    break;
                }

                // Next element
                element = element->suivant;
            }
            // On libere la liste
            liste_free(&liste, false);

            // Si on peut bouger
            if (canMove) {
                // On bouge
                argsMob->mob->x = newX;
                argsMob->mob->y = newY;
            }
        } else {
            // On bouge
            argsMob->mob->x = newX;
            argsMob->mob->y = newY;
        }

        // On regarde si il y a un joueur dans la hitbox
        size = objet_getSize(argsMob->mob);
        Liste listePlayer = playersInHitBox(sharedMemory, &argsMob->levelMutex->level, argsMob->mob->x, argsMob->mob->y, size.xSize, size.ySize);
        // Si la liste n'est pas vide alors on attaque
        if (listePlayer.taille > 0) {
            EltListe *element = listePlayer.tete;
            while (element != NULL) {
                Player *player = (Player*)element->elmt;

                if (player->isInvincible || !player->isAlive) {
                    // Next element
                    element = element->suivant;
                    continue;
                }

                if (player->life > 1) {
                    player->life--;
                    // Envoie d'un message au joueur
                    privateMessage(sharedMemory, player->numPlayer, "Une probe vous a attaqué !", YELLOW_COLOR, 1);
                    // Make invincible
                    player->isInvincible = true;
                    // On lance un thread pour le rendre non invincible
                    launch_uninvincible_player_routine(sharedMemory, player);
                } else {
                    player->life--;
                    death_player_routine(sharedMemory, player);
                }

                // Next element
                element = element->suivant;
            }
            
        }
        // On libere la liste
        liste_free(&listePlayer, false);

        // Signal condition variable
        pthread_cond_broadcast(&sharedMemory->update_cond);
        pthread_mutex_unlock(&sharedMemory->mutex);
        
        // Latence comme pour le joueur de 0.5s
        usleep(500000);
    }
    
    printf("Probe %d is dead\n", argsMob->mob->id);
    return NULL;
}

void* robot_routine(void* args) {
    // Recuperer les arguments
    void **argsTable = (void**)args;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)argsTable[0];
    MobThreadsArgs *argsMob = (MobThreadsArgs*)argsTable[1];
    // Free the arguments
    free(args);
    bool orientationLeft = true;
    printf("Debut routine robot\n");

    // Si jeux fini alors th_shared_memory.game_state = 2
    while (sharedMemory->game_state != 2) {
        // Lock the mutex
        pthread_mutex_lock(&sharedMemory->mutex);

        // Mise en pause si freeze et reveil sur condition
        if (argsMob->isFreeze) {
            pthread_mutex_unlock(&sharedMemory->mutex);
            printf("Robot %d is freeze\n", argsMob->mob->id);
            // On fait dormir le temps du freeze soit 5s
            sleep(5);
            printf("Robot %d is unfreeze\n", argsMob->mob->id);
            pthread_mutex_lock(&sharedMemory->mutex);
            argsMob->isFreeze = false;
        }

        // Mouvement du robot ajout de plus 1 pour la hitbox car on veut voir les pieges au sol
        int value = orientationLeft ? -1 : 1;
        Objet *objet = argsMob->mob;
        ObjetSize size = objet_getSize(objet);
        Liste block = objectInHitBox(&argsMob->levelMutex->level, argsMob->mob->x+value, argsMob->mob->y+1, size.xSize, size.ySize+1);
        EltListe *element = block.tete;
        bool canMove = true;
        while (element != NULL) {
            Objet *objet = (Objet*)element->elmt;
            if ((objet->y <= argsMob->mob->y && (objet->type == BLOCK_ID || objet->type == GATE_ID)) 
                || (objet->type == TRAP_ID)) {
                orientationLeft = !orientationLeft;
                canMove = false;
                break;
            }

            // Next element
            element = element->suivant;
        }

        // On verifie si il y a du vide sous les pieds
        if (canMove) {
            // Ajustment pour orientation droite car on veut pas un pied dans le vide et l'autre sur le block
            int orienAdd = orientationLeft ? 0 : +2;
            Liste vide = objectInHitBox(&argsMob->levelMutex->level, argsMob->mob->x+value+orienAdd, argsMob->mob->y+1, 1, 1);
            if (vide.tete == NULL) {
                orientationLeft = !orientationLeft;
                canMove = false;
            }
            // On libere la liste
            liste_free(&vide, false);
        }

        // On libere la liste
        liste_free(&block, false);

        // On change le x suivant l'orientation
        if (orientationLeft && canMove) {
            argsMob->mob->x--;
        } else if (canMove) {
            argsMob->mob->x++;
        }

        // On regarde si il y a un joueur dans la hitbox
        size = objet_getSize(argsMob->mob);
        Liste listePlayer = playersInHitBox(sharedMemory, &argsMob->levelMutex->level, argsMob->mob->x, argsMob->mob->y, size.xSize, size.ySize);
        // Si la liste n'est pas vide alors on attaque
        if (listePlayer.taille > 0) {
            EltListe *element = listePlayer.tete;
            while (element != NULL) {
                Player *player = (Player*)element->elmt;

                if (player->isInvincible || !player->isAlive) {
                    // Next element
                    element = element->suivant;
                    continue;
                }

                if (player->life > 1) {
                    player->life--;
                    // Envoie d'un message au joueur
                    privateMessage(sharedMemory, player->numPlayer, "Un robot vous a attaqué !", YELLOW_COLOR, 1);
                    // Make invincible
                    player->isInvincible = true;
                    // On lance un thread pour le rendre non invincible
                    launch_uninvincible_player_routine(sharedMemory, player);
                } else {
                    player->life--;
                    death_player_routine(sharedMemory, player);
                }

                // Next element
                element = element->suivant;
            }
            
        }
        // On libere la liste
        liste_free(&listePlayer, false);

        // Signal condition variable
        pthread_cond_broadcast(&sharedMemory->update_cond);
        pthread_mutex_unlock(&sharedMemory->mutex);
        
        // Latence comme pour le joueur de 0.01s
        usleep(400000);
    }

    printf("Robot %d is dead\n", argsMob->mob->id);
    return NULL;
}

void* piege_routine(void* args) {
    // Recuperer les arguments
    void **argsTable = (void**)args;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)argsTable[0];
    Liste *piegeThreadsArgs = (Liste*)argsTable[1];
    // Free the arguments
    free(args);

    logs(L_INFO, "Routine | Lancement routine piège");
    printf("Routine | Lancement routine piège\n");

    // Si jeu fini alors th_shared_memory.game_state = 2
    while (sharedMemory->game_state != 2) {
        // Lock the mutex
        pthread_mutex_lock(&sharedMemory->mutex);

        // Activation des pièges 
        EltListe *elt = piegeThreadsArgs->tete;
        while (elt != NULL) {
            PiegeLoaded *argsPiege = (PiegeLoaded*)elt->elmt;
            
            // Toggle du piege
            argsPiege->piege->trap.piegeActif = !argsPiege->piege->trap.piegeActif;
            
            if (argsPiege->piege->trap.piegeActif) {
                // On regarde si il y a un joueur dans la hitbox
                ObjetSize size = objet_getSize(argsPiege->piege);
                Liste listePlayer = playersInHitBox(sharedMemory, &argsPiege->levelMutex->level, argsPiege->piege->x, argsPiege->piege->y-1, size.xSize, size.ySize);
                // Si la liste n'est pas vide alors on attaque
                if (listePlayer.taille > 0) {
                    EltListe *element = listePlayer.tete;
                    while (element != NULL) {
                        Player *player = (Player*)element->elmt;

                        if (player->isInvincible || !player->isAlive) {
                            // Next element
                            element = element->suivant;
                            continue;
                        }

                        if (player->life > 1) {
                            player->life--;
                            // Envoie d'un message au joueur
                            privateMessage(sharedMemory, player->numPlayer, "Vous venez de vous prendre un piège !", YELLOW_COLOR, 1);
                            // Make invincible
                            player->isInvincible = true;
                            // On lance un thread pour le rendre non invincible
                            launch_uninvincible_player_routine(sharedMemory, player);
                        } else {
                            player->life--;
                            death_player_routine(sharedMemory, player);
                        }

                        // Next element
                        element = element->suivant;
                    }
                    
                }
                // On libere la liste
                liste_free(&listePlayer, false);
            }
            
            // Next element
            elt = elt->suivant;
        }

        // Signal condition variable
        pthread_cond_broadcast(&sharedMemory->update_cond);
        pthread_mutex_unlock(&sharedMemory->mutex);

        // Latence aleatoire entre 3 et 5s
        usleep((rand() % 2000000) + 3000000);
    }

    logs(L_INFO, "Routine | Fin routine piège");
    printf("Routine | Fin routine piège\n");
    return NULL;
}

void launch_mob_routine(threadsSharedMemory *sharedMemory, MobThreadsArgs *argsMobs) {
    if (argsMobs->mob->type == ROBOT_ID) {
        // Create table of arguments
        void **args = malloc(2 * sizeof(void*));
        args[0] = sharedMemory;
        args[1] = argsMobs;
        // Create the thread
        if (pthread_create(&argsMobs->thread, NULL, robot_routine, args) == -1) {
            perror("pthread_create");
            logs(L_INFO, "pthread_create failed %s", strerror(errno));
        }
    } else if (argsMobs->mob->type == PROBE_ID) {
        // Create table of arguments
        void **args = malloc(2 * sizeof(void*));
        args[0] = sharedMemory;
        args[1] = argsMobs;
        // Create the thread
        if (pthread_create(&argsMobs->thread, NULL, probe_routine, args) == -1) {
            perror("pthread_create");
            logs(L_INFO, "pthread_create failed %s", strerror(errno));
        }
    }
}

pthread_t launch_piege_routine(threadsSharedMemory *sharedMemory, Liste *piegeThreadsArgs) {
    // Create table of arguments
    void **args = malloc(2 * sizeof(void*));
    args[0] = sharedMemory;
    args[1] = piegeThreadsArgs;
    // Create the thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, piege_routine, args) == -1) {
        perror("pthread_create");
        logs(L_INFO, "pthread_create failed %s", strerror(errno));
    }
    return thread;
}

