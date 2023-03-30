
#ifndef LEVEL_EDIT_H
#define LEVEL_EDIT_H

#include "level.h"

typedef struct Position_s {
    short x;
    short y;
} Position;

int checkHitBox(Level* level, short x, short y, short xSize, short ySize);

Liste objectInHitBox(Level* level, short x, short y, short xSize, short ySize);
Liste playersInHitBox(Level* level, short x, short y, short xSize, short ySize);
Liste mobsAndPlayersInHitBox(Level* level, short x, short y, short xSize, short ySize);
Liste mobsInHitBox(Level* level, short x, short y, short xSize, short ySize);

Objet* poserPlayer(Level* level, short x, short y);

#endif

