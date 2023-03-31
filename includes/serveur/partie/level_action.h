
#ifndef __PLAYER8MOVEMENT__
#define __PLAYER8MOVEMENT__

#include "player.h"
#include "level.h"
#include "partie_manager.h"

void player_action(Player *player, LevelMutex *levelMutex, short newX, short newY, threadsSharedMemory *sharedMemory);

DoorLink* create_doorlink(Liste *doors);

void changePlayerOfLevel(threadsSharedMemory *sharedMemory, Player *player, LevelMutex *oldLevel, LevelMutex *newLevel, short x, short y);
void launch_bomb_routine(threadsSharedMemory *sharedMemory, Objet *obj, LevelMutex *levelMutex);
void launch_unfreeze_player_routine(threadsSharedMemory *sharedMemory, Player *player);

void death_player_routine(threadsSharedMemory *sharedMemory, Player *player);
void respawn_player_routine(threadsSharedMemory *sharedMemory, Player *player);

#endif // __PLAYER8MOVEMENT__

