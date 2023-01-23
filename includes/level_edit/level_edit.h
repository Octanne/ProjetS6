#ifndef LEVEL_EDIT_H
#define LEVEL_EDIT_H

#include "../level/level.h"

typedef struct Position_s {
    int x;
    int y;
} Position;

int checkHitBox(Level* level, int x, int y, int xSize, int ySize);

int supprimerObjet(Level* level, int x, int y);

int poserBlock(Level* level, int x, int y);
int poserVie(Level* level, int x, int y);
int poserBomb(Level* level, int x, int y);
int poserTrap(Level* level, int x, int y);
int poserGate(Level* level, int x, int y, int gateColor);
int poserKey(Level* level, int x, int y, int keyColor);
int poserDoor(Level* level, int x, int y, int doorNumber);
int poserExit(Level* level, int x, int y);
int poserStart(Level* level, int x, int y);
int poserProbe(Level* level, int x, int y);
int poserRobot(Level* level, int x, int y);
int poserLadder(Level* level, int x, int y);
int poserPlayer(Level* level, int x, int y);

#endif