#include "objet.h"

#include <stdlib.h>

#include "../constants.h"

Objet* initObjet(int x, int y, int xSize, int ySize, int type) {
    Objet* objet = malloc(sizeof(Objet));
    objet->type = type;
    objet->isActive = 1;

    objet->x = x;
    objet->y = y;

    objet->xSize = xSize;
    objet->ySize = ySize;

    return objet;
}

Objet* creerBlock(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, BLOCK_ID);
    
    objet->objet.block.x = &objet->x;
    objet->objet.block.y = &objet->y;

    return objet;
}

Objet* creerVie(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, HEART_ID);
    
    objet->objet.heart.x = &objet->x;
    objet->objet.heart.y = &objet->y;
    
    return objet;
}

Objet* creerBomb(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, BOMB_ID);
    
    objet->objet.bomb.x = &objet->x;
    objet->objet.bomb.y = &objet->y;
    return objet;
}

Objet* creerTrap(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, TRAP_ID);
    
    objet->objet.trap.x = &objet->x;
    objet->objet.trap.y = &objet->y;
    return objet;
}

Objet* creerGate(int x, int y, int numgate) {
    Objet* objet = initObjet(x, y, 1, 1, GATE_ID);
    
    objet->objet.gate.x = &objet->x;
    objet->objet.gate.y = &objet->y;
    objet->objet.gate.numgate = numgate;
    return objet;
}

Objet* creerKey(int x, int y, int numkey) {
    Objet* objet = initObjet(x, y, 1, 2, KEY_ID);
    
    objet->objet.key.x = &objet->x;
    objet->objet.key.y = &objet->y;
    objet->objet.key.numkey = numkey;
    return objet;
}

Objet* creerDoor(int x, int y, int numdoor) {
    Objet* objet = initObjet(x, y, 3, 4, DOOR_ID);
    
    objet->objet.door.x = &objet->x;
    objet->objet.door.y = &objet->y;
    objet->objet.door.numdoor = numdoor;
    return objet;
}

Objet* creerExit(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 4, EXIT_ID);
    
    objet->objet.exit.x = &objet->x;
    objet->objet.exit.y = &objet->y;
    return objet;
}

Objet* creerStart(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 4, START_ID);
    
    objet->objet.start.x = &objet->x;
    objet->objet.start.y = &objet->y;
    return objet;
}

Objet* creerLadder(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 1, LADDER_ID);
    
    objet->objet.ladder.x = &objet->x;
    objet->objet.ladder.y = &objet->y;
    return objet;
}

Objet* creerProbe(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 2, PROBE_ID);
    
    objet->objet.probe.x = &objet->x;
    objet->objet.probe.y = &objet->y;
    return objet;
}

Objet* creerRobot(int x, int y) {
    Objet* objet = initObjet(x, y, 4, 3, ROBOT_ID);

    objet->objet.robot.x = &objet->x;
    objet->objet.robot.y = &objet->y;
    return objet;
}

Objet* creerPlayer(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 3, PLAYER_ID);
    
    objet->objet.player.x = &objet->x;
    objet->objet.player.y = &objet->y;
    objet->objet.player.life = 3;
    
    objet->objet.player.orientation = RIGHT_ORIENTATION;
    objet->objet.player.color = LBLUE_COLOR;

    return objet;
}

void objet_free(Objet* objet) {
    if (objet == NULL) return;
    free(objet);
}