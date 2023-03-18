
#ifndef LEVEL_EDIT_H
#define LEVEL_EDIT_H

#include "level.h"

typedef struct Position_s {
    short x;
    short y;
} Position;

int checkHitBox(Level* level, short x, short y, short xSize, short ySize);

int supprimerObjet(Level* level, short x, short y);

int poserBlock(Level* level, short x, short y);
int poserVie(Level* level, short x, short y);
int poserBomb(Level* level, short x, short y);
int poserTrap(Level* level, short x, short y);
int poserGate(Level* level, short x, short y, int8_t gateColor);
int poserKey(Level* level, short x, short y, int8_t keyColor);
int poserDoor(Level* level, short x, short y, int8_t doorNumber);
int poserExit(Level* level, short x, short y);
int poserStart(Level* level, short x, short y);
int poserProbe(Level* level, short x, short y);
int poserRobot(Level* level, short x, short y);
int poserLadder(Level* level, short x, short y);
int poserPlayer(Level* level, short x, short y);

#endif

