#include "level_edit.h"

#include <stdlib.h>
#include <stdio.h>

#include "../utils/utils.h"

/**
 * @brief Check if a position is free.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * @param xSize : The x size of the hitbox.
 * @param ySize : The y size of the hitbox.
 * 
 * @return int : 1 if the position is free, 0 otherwise.
 * 
 * @c TODO revoir hitbox semble pas fonctionner correctement
 */
int checkHitBox(Level* level, short x, short y, short xSize, short ySize) {

	// Check Border collisions
	if (x + xSize > MATRICE_LEVEL_X || y - ySize > MATRICE_LEVEL_Y) {
		logs(L_DEBUG, "checkHitBox => x: %d, y: %d (collision avec le bord)", x, y);
		return 0;
	}

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position
			ListeObjet objs = rechercherObjet(level, i, j);
			
			// If the list is not empty, there is a collision
			if (objs.tete != NULL) {
				logs(L_DEBUG, "read => x: %d, y: %d (collision avec %d)", i, j, objs.tete->objet->type);
				return 0;
			}
			else
				logs(L_DEBUG, "read => x: %d, y: %d (pas de collision)", i, j);
			
			// Free the list
			listeObjet_free(&objs, 0);
		}
	}
	return 1;
}

/**
 * @brief Delete an object at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if an object was deleted, 0 otherwise.
 */
int supprimerObjet(Level* level, short x, short y) {
	ListeObjet objs = rechercherObjet(level, x, y);

	// If the list is not empty, delete the object
	if (objs.tete != NULL) {
		levelSupprimerObjet(level, objs.tete->objet);
		listeObjet_free(&objs, 0);
		return 1;
	}

	// Free the list and return 0 to indicate that no object was deleted
	listeObjet_free(&objs, 0);
	return 0;
}

/**
 * @brief Create a block at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a block was created, 0 otherwise.
 */
int poserBlock(Level* level, short x, short y) {

	// Logs & get objects at the position
	logs(L_DEBUG, "poserBlock(%d, %d)", x, y);
	ListeObjet objs = rechercherObjet(level, x, y);

	// If the list is empty, create objet
	if (objs.tete == NULL) {
		levelAjouterObjet(level, creerBlock(x, y));
		listeObjet_free(&objs, 0);
		return 1;
	}

	// Free the list and return 0 to indicate that no object was created
	listeObjet_free(&objs, 0);
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserVie(Level* level, short x, short y) {
	ListeObjet objs = rechercherObjet(level, x, y);

	// If the list is empty, create objet
	if (objs.tete == NULL) {
		levelAjouterObjet(level, creerVie(x, y));
		listeObjet_free(&objs, 0);
		return 1;
	}

	// Free the list and return 0 to indicate that no object was created
	listeObjet_free(&objs, 0);
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserBomb(Level* level, short x, short y) {
	ListeObjet objs = rechercherObjet(level, x, y);

	// If the list is empty, create objet
	if (objs.tete == NULL) {
		levelAjouterObjet(level, creerBomb(x, y));
		listeObjet_free(&objs, 0);
		return 1;
	}

	// Free the list and return 0 to indicate that no object was created
	listeObjet_free(&objs, 0);
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserTrap(Level* level, short x, short y) {
	ListeObjet objs = rechercherObjet(level, x, y);

	// If the list is empty, create objet
	if (objs.tete == NULL) {
		levelAjouterObjet(level, creerTrap(x, y));
		listeObjet_free(&objs, 0);
		return 1;
	}

	// Free the list and return 0 to indicate that no object was created
	listeObjet_free(&objs, 0);
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserGate(Level* level, short x, short y, int8_t gateColor) {
	ListeObjet objs = rechercherObjet(level, x, y);

	// If the list is empty, create objet
	if (objs.tete == NULL) {
		levelAjouterObjet(level, creerGate(x, y, gateColor));
		listeObjet_free(&objs, 0);
		return 1;
	}

	// Free the list and return 0 to indicate that no object was created
	listeObjet_free(&objs, 0);
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserKey(Level* level, short x, short y, int8_t keyColor) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 1, 2)) {
		levelAjouterObjet(level, creerKey(x, y, keyColor));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserDoor(Level* level, short x, short y, int8_t doorNumber) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 4)) {
		levelAjouterObjet(level, creerDoor(x, y, doorNumber));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserExit(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 4)) {
		levelAjouterObjet(level, creerExit(x, y));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserStart(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 4)) {
		levelAjouterObjet(level, creerStart(x, y));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserProbe(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 2)) {
		levelAjouterObjet(level, creerProbe(x, y));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserRobot(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 4, 3)) {
		levelAjouterObjet(level, creerRobot(x, y));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserLadder(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 1)) {
		levelAjouterObjet(level, creerLadder(x, y));
		return 1;
	}
	return 0;
}

/**
 * @brief Create a wall at a position.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return 1 if a wall was created, 0 otherwise.
 */
int poserPlayer(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 3)) {
		levelAjouterObjet(level, creerPlayer(x, y));
		return 1;
	}
	return 0;
}

