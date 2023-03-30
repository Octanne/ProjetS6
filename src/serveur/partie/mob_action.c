
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
        //pthread_mutex_lock(&argsMob->mutex);

        // Mise en pause si freeze et reveil sur condition
        if (argsMob->isFreeze) {
            printf("Probe %d is freeze\n", argsMob->mob->id);
            pthread_cond_wait(&argsMob->update_cond, &sharedMemory->mutex);
            printf("Probe %d is unfreeze\n", argsMob->mob->id);
        }

        // TODO : ajouter les mouvements du probe et l'attaque si sur un joueur
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
        Liste liste = objectInHitBox(argsMob->level, newX, newY, 3, 2);
        // Si la liste est vide alors on peut bouger
        if (liste.tete != NULL) {
            bool canMove = true;
            // On parcoure la liste pour voir si il y a un joueur
            EltListe *element = liste.tete;
            while (element != NULL) {
                Objet *objet = (Objet*)element->elmt;
                if (objet->type == BLOCK_ID || objet->type == GATE_ID ) {
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

        // Signal condition variable
        pthread_cond_broadcast(&sharedMemory->update_cond);
        pthread_mutex_unlock(&sharedMemory->mutex);
        //pthread_mutex_unlock(&argsMob->mutex);
        
        // Latence comme pour le joueur de 0.5s
        usleep(500000);
        printf("Routine probe !\n");
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

    printf("Debut routine robot\n");

    // Si jeux fini alors th_shared_memory.game_state = 2
    while (sharedMemory->game_state != 2) {
        // Lock the mutex
        pthread_mutex_lock(&sharedMemory->mutex);
        //pthread_mutex_lock(&argsMob->mutex);

        // Mise en pause si freeze et reveil sur condition
        if (argsMob->isFreeze) {
            printf("Robot %d is freeze\n", argsMob->mob->id);
            pthread_cond_wait(&argsMob->update_cond, &sharedMemory->mutex);
            printf("Robot %d is unfreeze\n", argsMob->mob->id);
        }

        // TODO : ajouter les mouvements du robot et l'attaque si sur un joueur

        // Signal condition variable
        pthread_cond_broadcast(&sharedMemory->update_cond);
        pthread_mutex_unlock(&sharedMemory->mutex);
        //pthread_mutex_unlock(&argsMob->mutex);
        
        // Latence comme pour le joueur de 0.01s
        sleep(1);
        printf("Routine Robot\n");
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
            PiegeThreadArgs *argsPiege = (PiegeThreadArgs*)elt->elmt;
            
            // Toggle du piege
            argsPiege->piege->trap.piegeActif = !argsPiege->piege->trap.piegeActif;
            
            if (argsPiege->piege->trap.piegeActif) {
                // TODO : ajouter l'attaque si un joueur au dessus du piege
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

