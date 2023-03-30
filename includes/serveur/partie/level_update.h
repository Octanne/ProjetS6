
#ifndef LEVEL_EDIT_H
#define LEVEL_EDIT_H

#include "level.h"
#include "liste.h"
#include "partie_manager.h"

Liste rechercherMobsInListe(Liste listeMobs, Level *level, short x, short y);
Liste rechercherPlayerInArray(threadsSharedMemory *sharedMemory, Level *level, short x, short y);

Liste playersInHitBox(threadsSharedMemory *sharedMemory, Level* level, short x, short y, short xSize, short ySize);
Liste mobsInHitBox(threadsSharedMemory *sharedMemory, Level* level, short x, short y, short xSize, short ySize);

int checkHitBox(Level* level, short x, short y, short xSize, short ySize);
Liste objectInHitBox(Level* level, short x, short y, short xSize, short ySize);
Objet* poserPlayer(Level* level, short x, short y);

#endif

