
#include "level.h"

#include <stdlib.h>

#include "../utils/utils.h"

/**
 * @brief Create an empty level.
 * 
 * @return an empty level.
 */
Level levelEmpty() {
    Level level;
    level.listeObjet = creerListeObjet();
    levelUpdateMatriceSprite(&level);
    return level;
}

/**
 * @brief Create a new sprite data
 * 
 * @param sprite : The character of the sprite
 * @param color : The color of the sprite
 * 
 * @return SpriteData : The sprite data
*/
SpriteData creerSpriteData(char sprite, int color) {
    SpriteData spriteD;
    spriteD.color = color;
    spriteD.specialChar = 0;
    spriteD.sprite = sprite;
    return spriteD;
}

/**
 * @brief Create a new sprite data with a special character
 * 
 * @param sprite : The character of the sprite
 * @param color : The color of the sprite
 * 
 * @return SpriteData : The sprite data
*/
SpriteData creerSpriteDataS(chtype sprite, int color) {
    SpriteData spriteD;
    spriteD.color = color;
    spriteD.specialChar = 1;
    spriteD.spSprite = sprite;
    return spriteD;
}

/**
 * @brief Create a new level.
 * 
 * @return Level : The new level.
*/
Level levelCreer() {
    Level level;
    level.listeObjet = creerListeObjet();

    // Ajouter les murs de la map
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
            if (y == 0 || y == MATRICE_LEVEL_Y - 1 || x == 0 || x == MATRICE_LEVEL_X - 1) {
                listeAjouterObjet(&level.listeObjet, creerBlock(x, y));
            }
        }
    }

	// Update the matriceSprite and return the level.
    levelUpdateMatriceSprite(&level);
    return level;
}

/**
 * @brief Free the memory of a level.
 * 
 * @param level : The level to free.
*/
void level_free(Level* level) {
    if (level == NULL) return;

	// Free the listeObjet
    listeObjet_free(&level->listeObjet, true);
}

/**
 * @brief Create an empty sprite data.
 * 
 * @return SpriteData : The empty sprite data.
*/
SpriteData emptySprite() {
    return creerSpriteData(' ', WHITE_COLOR);
}

/**
 * @brief Add an objet to the level.
 * 
 * @param level : The level.
 * @param objet : The objet to add.
*/
void levelAjouterObjet(Level* level, Objet* objet) {
    listeAjouterObjet(&level->listeObjet, objet);
    levelUpdateMatriceSprite(level);
    // TODO update CollideMatrice
}

/**
 * @brief Remove an objet from the level.
 * 
 * @param level : The level.
 * @param objet : The objet to remove.
*/
void levelSupprimerObjet(Level* level, Objet* objet) {
    listeSupprimerObjet(&level->listeObjet, objet, true);
    levelUpdateMatriceSprite(level);
    // TODO update CollideMatrice
}

/**
 * @brief Search an objet in the level.
 * 
 * @param level : The level.
 * @param x : The x position of the objet.
 * @param y : The y position of the objet.
 * 
 * @return ListeObjet* : The list of objet found (can be empty)
*/
ListeObjet rechercherObjet(Level* level, short x, short y) {
    ListeObjet liste = creerListeObjet();
    
	// Search in the list of objet of the level
    EltListe_o *elt = level->listeObjet.tete;
    while (elt != NULL) {

		// Check if x and y are in the zone delimited by the coordinates of the objet and its size.
        Objet* objet = elt->objet;
        if ( (x >= objet->x && x < objet->x + objet->xSize) && 
             (y <= objet->y && y > objet->y - objet->ySize)) {
            listeAjouterObjet(&liste, objet);
        }
        elt = elt->suivant;
    }

	// Return the list of objet found.
    return liste;
}

/**
 * @brief Update the matriceSprite of the level.
*/
void levelUpdateMatriceSprite(Level* level) {

    // Empty all the matriceSprite
	SpriteData empty = emptySprite();
	int i;
	for (i = 0; i < MATRICE_LEVEL_SIZE; i++) {
		level->matriceSprite[i] = empty;
	}

    // Parcourir les objets du level
    EltListe_o *elt = level->listeObjet.tete;
    while (elt != NULL) {
        char sprite;
        Objet* objet = elt->objet;
        int color, colorB;
        short x, y;

		// Switch sur le type de l'objet
        switch (objet->type) {
            case BLOCK_ID :
                sprite = ' ';
                level->matriceSprite[objet->y + objet->x*MATRICE_LEVEL_Y] = creerSpriteData(sprite, LBLUE_BLOCK);
                break;
            case HEART_ID :
                sprite = 'V';
                level->matriceSprite[objet->y + objet->x*MATRICE_LEVEL_Y] = creerSpriteData(sprite, RED_COLOR);
                break;
            case TRAP_ID :
                sprite = '#';
                level->matriceSprite[objet->y + objet->x*MATRICE_LEVEL_Y] = creerSpriteData(sprite, LBLUE_BLOCK);
                break;
            case BOMB_ID :
                sprite = 'o';
                level->matriceSprite[objet->y + objet->x*MATRICE_LEVEL_Y] = creerSpriteData(sprite, WHITE_COLOR);
                break;
            case GATE_ID :
                color = WHITE_COLOR;
                if (objet->objet.gate.numgate == 0) color = PURPLE_COLOR;
                else if (objet->objet.gate.numgate == 1) color = GREEN_COLOR;
                else if (objet->objet.gate.numgate == 2) color = YELLOW_COLOR;
                else if (objet->objet.gate.numgate == 3) color = LBLUE_COLOR;
                level->matriceSprite[objet->y + objet->x*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_PLUS, color);
                break;
            case KEY_ID :
                color = WHITE_COLOR;
                colorB = WHITE_COLOR;
                if (objet->objet.key.numkey == 0) {
                    color = PURPLE_COLOR;
                    colorB = PURPLE_BLOCK;
                } else if (objet->objet.key.numkey == 1) {
                    color = GREEN_COLOR;
                    colorB = GREEN_BLOCK;
                } else if (objet->objet.key.numkey == 2) {
                     color = YELLOW_COLOR;
                     colorB = YELLOW_BLOCK;
                } else if (objet->objet.key.numkey == 3) {
                    color = LBLUE_COLOR;
                    colorB = LBLUE_BLOCK;
                }
                // Sprite #1
                level->matriceSprite[objet->y-1 + objet->x*MATRICE_LEVEL_Y] = creerSpriteData(' ', colorB);;
                // Sprite #2
                level->matriceSprite[objet->y + objet->x*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LLCORNER, color);
                break; 
            case DOOR_ID:
                for (y = 0; y < objet->ySize; y++) {
                    for (x = 0; x < objet->xSize; x++) {
                        // Lead 0
                        if (y == 3 && x < 2) {
                            int8_t numDoor = objet->objet.door.numdoor;
                            char numDoorChar[4];
                            sprintf(numDoorChar, "%02i", numDoor);
                            if (x == 0) level->matriceSprite[(objet->y-y) + (objet->x+x)*MATRICE_LEVEL_Y] = creerSpriteData(numDoorChar[0], WHITE_COLOR);
                            if (x == 1) level->matriceSprite[(objet->y-y) + (objet->x+x)*MATRICE_LEVEL_Y] = creerSpriteData(numDoorChar[1], WHITE_COLOR);
                        } else level->matriceSprite[(objet->y-y) + (objet->x+x)*MATRICE_LEVEL_Y] = creerSpriteData(' ', GREEN_BLOCK);
                    }
                }
                break;
            case EXIT_ID :
                for (y = 0; y < objet->ySize; y++) {
                    for (x = 0; x < objet->xSize; x++) {
                        level->matriceSprite[(objet->y-y) + (objet->x+x)*MATRICE_LEVEL_Y] = creerSpriteData(' ', YELLOW_BLOCK);
                    }
                }
                break;
            case START_ID :
                for (y = 0; y < objet->ySize; y++) {
                    for (x = 0; x < objet->xSize; x++) {
                        level->matriceSprite[(objet->y-y) + (objet->x+x)*MATRICE_LEVEL_Y] = creerSpriteData(' ', PURPLE_BLOCK);
                    }
                }
                break;
            case LADDER_ID :
                level->matriceSprite[objet->y + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LTEE, YELLOW_COLOR);
                level->matriceSprite[objet->y + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, YELLOW_COLOR);
                level->matriceSprite[objet->y + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_RTEE, YELLOW_COLOR);
                break;
            case PROBE_ID :
                level->matriceSprite[objet->y-1 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LTEE, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_RTEE, WHITE_COLOR);

                level->matriceSprite[objet->y + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LLCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LRCORNER, WHITE_COLOR);
                break;
            case ROBOT_ID :
                level->matriceSprite[objet->y-3 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_ULCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y-3 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_BTEE, WHITE_COLOR);
                level->matriceSprite[objet->y-3 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_URCORNER, WHITE_COLOR);

                level->matriceSprite[objet->y-2 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LLCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y-2 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_TTEE, WHITE_COLOR);
                level->matriceSprite[objet->y-2 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_LRCORNER, WHITE_COLOR);

                level->matriceSprite[objet->y-1 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_PLUS, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);

                level->matriceSprite[objet->y + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_ULCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_BTEE, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_URCORNER, WHITE_COLOR);
                break;
            case PLAYER_ID :
                color = objet->objet.player.color;
                // head
                if (objet->objet.player.orientation == LEFT_ORIENTATION) {
                    level->matriceSprite[objet->y-3 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteData(' ', RED_BLOCK);
                    level->matriceSprite[objet->y-3 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteData('-', RED_BLOCK);
                } else {
                    level->matriceSprite[objet->y-3 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteData(' ', RED_BLOCK);
                    level->matriceSprite[objet->y-3 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteData('-', RED_BLOCK);
                }

                // body
                level->matriceSprite[objet->y-2 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, color);
                level->matriceSprite[objet->y-2 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_PLUS, color);
                level->matriceSprite[objet->y-2 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_HLINE, color);

                level->matriceSprite[objet->y-1 + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_ULCORNER, color);
                level->matriceSprite[objet->y-1 + (objet->x+1)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_BTEE, color);
                level->matriceSprite[objet->y-1 + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_URCORNER, color);

                level->matriceSprite[objet->y + (objet->x)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_VLINE, color);
                level->matriceSprite[objet->y + (objet->x+2)*MATRICE_LEVEL_Y] = creerSpriteDataS(ACS_VLINE, color);
                break;
        }
        elt = elt->suivant;
    }
}

