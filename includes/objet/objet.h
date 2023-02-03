#ifndef OBJET_H
#define OBJET_H

#include <stdint.h>

typedef struct {
} Block_obj;

typedef struct {
} Ladder_obj;

typedef struct {
} Trap_obj;

typedef struct {
    int8_t numkey;
} Key_obj;

typedef struct {
    int8_t numgate;
} Gate_obj;

typedef struct {
    int8_t numdoor;
} Door_obj;

typedef struct {
} Exit_obj;

typedef struct {
} Start_obj;

typedef struct {
} Robot_obj;

typedef struct {
} Probe_obj;

typedef struct {
} Heart_obj;

typedef struct {
} Bomb_obj;

typedef struct {
    int8_t life;
    int8_t color;
    int8_t orientation;
} Player_obj;

typedef union {
    Block_obj block;
    Heart_obj heart;
    Trap_obj trap;
    Bomb_obj bomb;
    Gate_obj gate;
    Key_obj key;
    Door_obj door;
    Exit_obj exit;
    Start_obj start;
    Ladder_obj ladder;
    Probe_obj probe;
    Robot_obj robot;
    Player_obj player;
} ObjetU;

typedef struct {
    int8_t type;
    ObjetU objet;

    short xSize;
    short ySize;

    short x;
    short y;

    int8_t isActive;
} Objet;

Objet* initObjet(short x, short y, short xSize, short ySize, int8_t type);

Objet* creerVie(short x, short y);
Objet* creerRobot(short x, short y);
Objet* creerProbe(short x, short y);
Objet* creerBlock(short x, short y);
Objet* creerLadder(short x, short y);
Objet* creerTrap(short x, short y);
Objet* creerKey(short x, short y, int8_t numkey);
Objet* creerGate(short x, short y, int8_t numgate);
Objet* creerDoor(short x, short y, int8_t numdoor);
Objet* creerExit(short x, short y);
Objet* creerStart(short x, short y);
Objet* creerBomb(short x, short y);
Objet* creerPlayer(short x, short y);

void objet_free(Objet* objet);

#endif