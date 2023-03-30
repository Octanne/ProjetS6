
#ifndef __MOB_ACTION_H__
#define __MOB_ACTION_H__

#include <pthread.h>

#include "partie_manager.h"
#include "liste.h"

void launch_mob_routine(threadsSharedMemory *sharedMemory, MobThreadsArgs *argsMobs);
pthread_t launch_piege_routine(threadsSharedMemory *sharedMemory, Liste *piegeThreadsArgs);

void launch_uninvincible_player_routine(threadsSharedMemory *sharedMemory, Player *player);

#endif // __MOB_ACTION_H__

