
#include "objet.h"
#include "utils.h"
#include "constants.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Init an object with the given parameters.
 * 
 * @param x		: X position of the object.
 * @param y		: Y position of the object.
 * @param xSize	: X size of the object.
 * @param ySize	: Y size of the object.
 * @param type	: Type of the object.
 * 
 * @return Objet* : The object.
 */
Objet* initObjet(short x, short y, int8_t type) {
    Objet* objet = malloc(sizeof(Objet));
	if (objet == NULL) {
		logs(L_DEBUG, "initObjet | ERROR malloc objet");
		perror("Error while allocating memory in initObjet\n");
		exit(EXIT_FAILURE);
	}
    objet->type = type;
    objet->isActive = true;

    objet->x = x;
    objet->y = y;
    objet->id = -1;

    return objet;
}

Objet* creerBlock(short x, short y) { return initObjet(x, y, BLOCK_ID); }
Objet* creerExit(short x, short y) { return initObjet(x, y, EXIT_ID); }
Objet* creerStart(short x, short y) { return initObjet(x, y, START_ID); }
Objet* creerLadder(short x, short y) { return initObjet(x, y, LADDER_ID); }
Objet* creerProbe(short x, short y) { return initObjet(x, y, PROBE_ID); }
Objet* creerRobot(short x, short y) { return initObjet(x, y, ROBOT_ID); }
Objet* creerBombeExplosif(short x, short y) { return initObjet(x, y, BOMB_EXPLOSIVE_ID); }

Objet* creerTrap(short x, short y) {
    Objet *objet = initObjet(x, y, TRAP_ID);
    objet->trap.piegeActif = true;
    return objet;
}

Objet* creerGate(short x, short y, int8_t numgate) {
	Objet *objet = initObjet(x, y, GATE_ID);
    objet->gate.numgate = numgate;
    return objet;
}

Objet* creerKey(short x, short y, int8_t numkey) {
	Objet *objet = initObjet(x, y, KEY_ID);
    objet->key.numkey = numkey;
    return objet;
}

Objet* creerDoor(short x, short y, int8_t numdoor) {
	Objet *objet = initObjet(x, y, DOOR_ID);
    objet->door.numdoor = numdoor;
    return objet;
}

Objet* creerPlayer(short x, short y) {
    Objet* objet = initObjet(x, y, PLAYER_ID);
    objet->player.orientation = RIGHT_ORIENTATION;
    objet->player.color = LBLUE_COLOR;
    return objet;
}

Objet* creerVie(short x, short y) { 
    Objet* objet =  initObjet(x, y, HEART_ID);
    return objet;
}

Objet* creerBomb(short x, short y) { 
    Objet* objet =  initObjet(x, y, BOMB_ID);
    return objet;
}

/**
 * @brief Free allocated memory for an Objet
 */
void objet_free(Objet* objet) {
    if (objet == NULL)
		return;
    free(objet);
}

ObjetSize objet_getSize(Objet* objet) {
    ObjetSize size;
    switch (objet->type) {
        case BLOCK_ID:
        case HEART_ID:
        case BOMB_ID:
        case TRAP_ID:
        case BOMB_EXPLOSIVE_ID:
        case GATE_ID:
            size.xSize = 1;
            size.ySize = 1;
            break;
        case KEY_ID:
            size.xSize = 1;
            size.ySize = 2;
            break;
        case EXIT_ID:
        case START_ID:
        case DOOR_ID:
            size.xSize = 3;
            size.ySize = 4;
            break;
        case LADDER_ID:
            size.xSize = 3;
            size.ySize = 1;
            break;
        case PROBE_ID:
            size.xSize = 3;
            size.ySize = 2;
            break;
        case ROBOT_ID:
            size.xSize = 4;
            size.ySize = 3;
            break;
        case PLAYER_ID:
            size.xSize = 3;
            size.ySize = 4;
            break;
        default:
            logs(L_INFO, "objet_getSize | ERROR unknown type");
            size.xSize = 0;
            size.ySize = 0;
            break;
    }

    return size;
}

