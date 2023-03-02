
#include "objet.h"

#include <stdlib.h>

#include "../utils/utils.h"

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
Objet* initObjet(short x, short y, short xSize, short ySize, int8_t type) {
    Objet* objet = malloc(sizeof(Objet));
	if (objet == NULL) {
		logs(L_DEBUG, "initObjet | ERROR malloc objet");
		perror("Error while allocating memory in initObjet\n");
		exit(EXIT_FAILURE);
	}
    objet->type = type;
    objet->isActive = 1;

    objet->x = x;
    objet->y = y;

    objet->xSize = xSize;
    objet->ySize = ySize;

    return objet;
}

Objet* creerBlock(short x, short y) { return initObjet(x, y, 1, 1, BLOCK_ID); }
Objet* creerVie(short x, short y) { return initObjet(x, y, 1, 1, HEART_ID); }
Objet* creerBomb(short x, short y) { return initObjet(x, y, 1, 1, BOMB_ID); }
Objet* creerTrap(short x, short y) { return initObjet(x, y, 1, 1, TRAP_ID); }
Objet* creerExit(short x, short y) { return initObjet(x, y, 3, 4, EXIT_ID); }
Objet* creerStart(short x, short y) { return initObjet(x, y, 3, 4, START_ID); }
Objet* creerLadder(short x, short y) { return initObjet(x, y, 3, 1, LADDER_ID); }
Objet* creerProbe(short x, short y) { return initObjet(x, y, 3, 2, PROBE_ID); }
Objet* creerRobot(short x, short y) { return initObjet(x, y, 4, 3, ROBOT_ID); }

Objet* creerGate(short x, short y, int8_t numgate) {
	Objet *objet = initObjet(x, y, 1, 1, GATE_ID);
    objet->objet.gate.numgate = numgate;
    return objet;
}

Objet* creerKey(short x, short y, int8_t numkey) {
	Objet *objet = initObjet(x, y, 1, 2, KEY_ID);
    objet->objet.key.numkey = numkey;
    return objet;
}

Objet* creerDoor(short x, short y, int8_t numdoor) {
	Objet *objet = initObjet(x, y, 3, 4, DOOR_ID);
    objet->objet.door.numdoor = numdoor;
    return objet;
}

Objet* creerPlayer(short x, short y) {
    Objet* objet = initObjet(x, y, 3, 3, PLAYER_ID);
    objet->objet.player.life = 3;
    objet->objet.player.orientation = RIGHT_ORIENTATION;
    objet->objet.player.color = LBLUE_COLOR;
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

