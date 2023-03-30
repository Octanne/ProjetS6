
#ifndef __PLAYER8MOVEMENT__
#define __PLAYER8MOVEMENT__

#include "player.h"
#include "level.h"
#include "partie_manager.h"

void player_action(Player *player, Level *level, short newX, short newY, threadsSharedMemory *sharedMemory);

DoorLink* create_doorlink(Liste *doors);

void changePlayerOfLevel(threadsSharedMemory *sharedMemory, Player *player, Level *oldLevel, Level *newLevel, short x, short y);
void launch_bomb_routine(threadsSharedMemory *sharedMemory, Objet *obj, Level *lvl);
void launch_unfreeze_player_routine(threadsSharedMemory *sharedMemory, Player *player);

#endif // __PLAYER8MOVEMENT__

