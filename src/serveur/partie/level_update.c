
#include "level_update.h"
#include "utils.h"
#include "constants.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Check if a position is free.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * @param xSize : The x size of the hitbox.
 * @param ySize : The y size of the hitbox.
 * 
 * @return Liste : A list of objects in the hitbox.
 * 
 */
Liste objectInHitBox(Level* level, short x, short y, short xSize, short ySize) {
	Liste returnObjs = liste_create(false);

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position of the hitbox
			Liste objs = rechercherObjet(level, i, j);
			
			EltListe *elmt = objs.tete;
			// Iterate over the list and add the objects to the return list
			while (elmt != NULL) {
				// Check if the object is not already in the list
				if (!liste_contains(&returnObjs, elmt->elmt)) {
					// Add the object to the list
					liste_add(&returnObjs, elmt->elmt, TYPE_OBJET);
				}

				// Next element
				elmt = elmt->suivant;
			}
			
			// Free the list
			liste_free(&objs, 0);
		}
	}

	return returnObjs;
}

/**
 * @brief Get a list of all the mobs and players in a hitbox.
 * 
 * @param level The level.
 * @param x the x position.
 * @param y the y position.
 * @param xSize The x size of the hitbox. 
 * @param ySize The y size of the hitbox.
 * @return Liste mobs and players in the hitbox.
 */
Liste mobsAndPlayersInHitBox(Level* level, short x, short y, short xSize, short ySize) {
	Liste returnObjs = liste_create(false);

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position of the hitbox
			Liste objs = rechercherObjet(level, i, j);
			
			EltListe *elmt = objs.tete;
			// Iterate over the list and add the objects to the return list
			while (elmt != NULL) {
				// Get the object
				Objet* obj = (Objet*)elmt->elmt;
				// Check if the object is not already in the list
				if (!liste_contains(&returnObjs, elmt->elmt) && (obj->type == PLAYER_ID || 
							obj->type == PROBE_ID || obj->type == ROBOT_ID)) {
					// Add the object to the list
					liste_add(&returnObjs, elmt->elmt, TYPE_OBJET);
				}

				// Next element
				elmt = elmt->suivant;
			}
			
			// Free the list
			liste_free(&objs, 0);
		}
	}

	return returnObjs;
}

/**
 * @brief Get a list of all the mobs in a hitbox.
 * 
 * @param level The level.
 * @param x the x position.
 * @param y the y position.
 * @param xSize The x size of the hitbox. 
 * @param ySize The y size of the hitbox.
 * @return Liste mobs and players in the hitbox.
 */
Liste mobsInHitBox(Level* level, short x, short y, short xSize, short ySize) {
	Liste returnObjs = liste_create(false);

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position of the hitbox
			Liste objs = rechercherObjet(level, i, j);
			
			EltListe *elmt = objs.tete;
			// Iterate over the list and add the objects to the return list
			while (elmt != NULL) {
				// Get the object
				Objet* obj = (Objet*)elmt->elmt;
				// Check if the object is not already in the list
				if (!liste_contains(&returnObjs, elmt->elmt) && (obj->type == PROBE_ID || obj->type == ROBOT_ID)) {
					// Add the object to the list
					liste_add(&returnObjs, elmt->elmt, TYPE_OBJET);
				}

				// Next element
				elmt = elmt->suivant;
			}
			
			// Free the list
			liste_free(&objs, 0);
		}
	}

	return returnObjs;
}

/**
 * @brief Get a list of all the players in a hitbox.
 * 
 * @param level The level.
 * @param x the x position.
 * @param y the y position.
 * @param xSize The x size of the hitbox. 
 * @param ySize The y size of the hitbox.
 * @return Liste players in the hitbox.
 */
Liste playersInHitBox(Level* level, short x, short y, short xSize, short ySize) {
	Liste returnObjs = liste_create(false);

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position of the hitbox
			Liste objs = rechercherObjet(level, i, j);
			
			EltListe *elmt = objs.tete;
			// Iterate over the list and add the objects to the return list
			while (elmt != NULL) {
				// Get the object
				Objet* obj = (Objet*)elmt->elmt;
				// Check if the object is not already in the list
				if (!liste_contains(&returnObjs, elmt->elmt) && obj->type == PLAYER_ID) {
					// Add the object to the list
					liste_add(&returnObjs, elmt->elmt, TYPE_OBJET);
				}

				// Next element
				elmt = elmt->suivant;
			}
			
			// Free the list
			liste_free(&objs, 0);
		}
	}

	return returnObjs;
}

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
			Liste objs = rechercherObjet(level, i, j);
			
			// If the list is not empty, there is a collision
			if (objs.tete != NULL) {

				// Collision only with block, robot, probe
				Objet* obj = (Objet*)objs.tete->elmt;
				if (obj->isActive && (obj->type == BLOCK_ID || obj->type == ROBOT_ID || obj->type == PROBE_ID)) {
					logs(L_DEBUG, "read => x: %d, y: %d (collision avec %d)", i, j, ((Objet*)objs.tete->elmt)->type);
					return 0;
				}
			}
			else
				logs(L_DEBUG, "read => x: %d, y: %d (pas de collision)", i, j);
			
			// Free the list
			liste_free(&objs, 0);
		}
	}
	return 1;
}

/**
 * @brief Place a player at a position and returns it.
 * 
 * @param level : The level.
 * @param x : The x position.
 * @param y : The y position.
 * 
 * @return The player if it was created, NULL otherwise.
 */
Objet* poserPlayer(Level* level, short x, short y) {

	// If the hitbox is free, create objet
	if (checkHitBox(level, x, y, 3, 4)) {
		Objet* player = creerPlayer(x, y);
		levelAjouterObjet(level, player);
		return player;
	}
	return NULL;
}

