#ifndef OBJET_H
#define OBJET_H

typedef struct {
} Block_obj;

typedef struct {
} Ladder_obj;

typedef struct {
} Trap_obj;

typedef struct {
    int numkey;
} Key_obj;

typedef struct {
    int numgate;
} Gate_obj;

typedef struct {
    int numdoor;
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
    int life;
    int color;
    int orientation;
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
    int type;
    ObjetU objet;

    int xSize;
    int ySize;

    int x;
    int y;

    int isActive;
} Objet;

Objet* initObjet(int x, int y, int xSize, int ySize, int type);

Objet* creerVie(int x, int y);
Objet* creerRobot(int x, int y);
Objet* creerProbe(int x, int y);
Objet* creerBlock(int x, int y);
Objet* creerLadder(int x, int y);
Objet* creerTrap(int x, int y);
Objet* creerKey(int x, int y, int numkey);
Objet* creerGate(int x, int y, int numgate);
Objet* creerDoor(int x, int y, int numdoor);
Objet* creerExit(int x, int y);
Objet* creerStart(int x, int y);
Objet* creerBomb(int x, int y);
Objet* creerPlayer(int x, int y);

void objet_free(Objet* objet);

#endif