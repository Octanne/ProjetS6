
#include "mob_action.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

void* probe_routine(void * args) {
    // Recuperer les arguments
    void **argsTable = (void**)args;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)argsTable[0];
    MobThreadsArgs *argsMobs = (MobThreadsArgs*)argsTable[1];
    
    // Lock the mutex
    pthread_mutex_lock(&sharedMemory->mutex);

    // TODO: Implement probe_routine

    // Signal condition variable
    pthread_cond_broadcast(&sharedMemory->update_cond);
    pthread_mutex_unlock(&sharedMemory->mutex);
    // Free the arguments
    free(args);

    printf("Probe %d is dead\n", argsMobs->mob->id);

    return NULL;
}

void* robot_routine(void* args) {
    // Recuperer les arguments
    void **argsTable = (void**)args;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)argsTable[0];
    MobThreadsArgs *argsMobs = (MobThreadsArgs*)argsTable[1];

    // Lock the mutex
    pthread_mutex_lock(&sharedMemory->mutex);


    // TODO: Implement robot_routine

    // Signal condition variable
    pthread_cond_broadcast(&sharedMemory->update_cond);
    pthread_mutex_unlock(&sharedMemory->mutex);
    // Free the arguments
    free(args);

    printf("Robot %d is dead\n", argsMobs->mob->id);

    return NULL;
}

void* piege_routine(void* args) {
    // Recuperer les arguments
    void **argsTable = (void**)args;
    threadsSharedMemory *sharedMemory = (threadsSharedMemory*)argsTable[0];
    Liste *piegeThreadsArgs = (Liste*)argsTable[1];

    // Make to avoid unused variable warning
    piegeThreadsArgs = piegeThreadsArgs;

    // TODO: Implement piege_routine

    // Signal condition variable
    pthread_cond_broadcast(&sharedMemory->update_cond);
    pthread_mutex_unlock(&sharedMemory->mutex);
    // Free the arguments
    free(args);

    printf("Piege routine is dead\n");

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

