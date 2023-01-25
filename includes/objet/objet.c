#include "objet.h"

#include <stdlib.h>

#include "../utils/utils.h"

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

    return objet;
}

Objet* creerVie(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, HEART_ID);
    
    return objet;
}

Objet* creerBomb(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, BOMB_ID);

    return objet;
}

Objet* creerTrap(int x, int y) {
    Objet* objet = initObjet(x, y, 1, 1, TRAP_ID);
    
    return objet;
}

Objet* creerGate(int x, int y, int numgate) {
    Objet* objet = initObjet(x, y, 1, 1, GATE_ID);
    
    objet->objet.gate.numgate = numgate;
    return objet;
}

Objet* creerKey(int x, int y, int numkey) {
    Objet* objet = initObjet(x, y, 1, 2, KEY_ID);
    
    objet->objet.key.numkey = numkey;
    return objet;
}

Objet* creerDoor(int x, int y, int numdoor) {
    Objet* objet = initObjet(x, y, 3, 4, DOOR_ID);
    
    objet->objet.door.numdoor = numdoor;
    return objet;
}

Objet* creerExit(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 4, EXIT_ID);
    
    return objet;
}

Objet* creerStart(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 4, START_ID);
    
    return objet;
}

Objet* creerLadder(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 1, LADDER_ID);
    
    return objet;
}

Objet* creerProbe(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 2, PROBE_ID);
    
    return objet;
}

Objet* creerRobot(int x, int y) {
    Objet* objet = initObjet(x, y, 4, 3, ROBOT_ID);

    return objet;
}

Objet* creerPlayer(int x, int y) {
    Objet* objet = initObjet(x, y, 3, 3, PLAYER_ID);
    
    objet->objet.player.life = 3;
    
    objet->objet.player.orientation = RIGHT_ORIENTATION;
    objet->objet.player.color = LBLUE_COLOR;

    return objet;
}

void objet_free(Objet* objet) {
    if (objet == NULL) return;
    free(objet);
}