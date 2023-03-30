
#include "level_update.h"
#include "utils.h"
#include "constants.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Récupère les mobs de la liste qui sont au coordonnées x et y.
 * 
 * @param listeMobs : La liste des mobs a parcourir.
 * @param level : Le niveau.
 * @param x : La position x.
 * @param y : La position y.
 * @return Liste : La liste des mobs trouvés.
 */
Liste rechercherMobsInListe(Liste listeMobs, Level *level, short x, short y) {
    Liste liste = liste_create(false);
    
	// Search in the list of objet of the level
    EltListe *elt = listeMobs.tete;
    while (elt != NULL) {
		// Check if x and y are in the zone delimited by the coordinates of the objet and its size.
		MobThreadsArgs *mob = (MobThreadsArgs*)elt->elmt;
        Objet* objet = mob->mob;
        ObjetSize size = objet_getSize(objet);
        if (mob->level == level && (x >= objet->x && x < objet->x + size.xSize) && 
             (y <= objet->y && y > objet->y - size.ySize)) {
            liste_add(&liste, mob, TYPE_MOBTHREAD_ARGS);
        }
        elt = elt->suivant;
    }

	// Return the list of objet found.
    return liste;
}

/**
 * @brief Récupère les joueurs de la liste qui sont au coordonnées x et y.
 * 
 * @param sharedMemory : La mémoire partagée.
 * @param level : Le niveau.
 * @param x : La position x.
 * @param y : La position y.
 * @return Liste 
 */
Liste rechercherPlayerInArray(threadsSharedMemory *sharedMemory, Level *level, short x, short y) {
    Liste liste = liste_create(false);
    
	// Search in the array of players
	int i;
	for (i = 0; i < sharedMemory->nbThreads; i++) {
		Objet *player = sharedMemory->players[i].obj;
		if (player != NULL) {
			ObjetSize size = objet_getSize(player);
			// Check if x and y are in the zone delimited by the coordinates of the objet and its size.
			if (sharedMemory->players[i].level-1 == level->levelNumber && (x >= player->x && x < player->x + size.xSize) && 
				 (y <= player->y && y > player->y - size.ySize)) {
				liste_add(&liste, &sharedMemory->players[i], TYPE_PLAYER);
			}
		}
	}

	// Return the list of objet found.
    return liste;
}

/**
 * @brief Retourne la liste des joueurs dans la hitbox donnée.
 * 
 * @param sharedMemory : La mémoire partagée.
 * @param level : Le niveau.
 * @param x : La position x.
 * @param y : La position y.
 * @param xSize : La taille x.
 * @param ySize : La taille y.
 * @return Liste : La liste des joueurs dans la hitbox.
 */
Liste playersInHitBox(threadsSharedMemory *sharedMemory, Level* level, short x, short y, short xSize, short ySize) {
	Liste returnObjs = liste_create(false);

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position of the hitbox
			Liste objs = rechercherPlayerInArray(sharedMemory, level, i, j);
			
			EltListe *elmt = objs.tete;
			// Iterate over the list and add the objects to the return list
			while (elmt != NULL) {
				// Check if the object is not already in the list
				if (!liste_contains(&returnObjs, elmt->elmt)) {
					// Add the object to the list
					liste_add(&returnObjs, elmt->elmt, TYPE_PLAYER);
				}

				// Next element
				elmt = elmt->suivant;
			}
		}
	}

	// Return the list of objet found.
	return returnObjs;
}

/**
 * @brief Retourne la liste des mobs dans la hitbox donnée.
 * 
 * @param sharedMemory : La mémoire partagée.
 * @param level : Le niveau.
 * @param x : La position x.
 * @param y : La position y.
 * @param xSize : La taille x.
 * @param ySize : La taille y.
 * @return Liste : La liste des mobs dans la hitbox.
 */
Liste mobsInHitBox(threadsSharedMemory *sharedMemory, Level* level, short x, short y, short xSize, short ySize) {
	Liste returnObjs = liste_create(false);

	// Check Object collisions by checking all positions of the hitbox
	short i, j;
	for (j = y; j > y-ySize; j--) {
		for (i = x; i < x+xSize; i++) {
			// Create a list of objects at the position of the hitbox
			Liste objs = rechercherMobsInListe(sharedMemory->mobsThreadsArgs, level, i, j);
			
			EltListe *elmt = objs.tete;
			// Iterate over the list and add the objects to the return list
			while (elmt != NULL) {
				// Check if the object is not already in the list
				if (!liste_contains(&returnObjs, elmt->elmt)) {
					// Add the object to the list
					liste_add(&returnObjs, elmt->elmt, TYPE_PLAYER);
				}

				// Next element
				elmt = elmt->suivant;
			}
		}
	}

	// Return the list of objet found.
	return returnObjs;
}

/**
 * @brief Retourne la liste des objets dans la hitbox donnée.
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

