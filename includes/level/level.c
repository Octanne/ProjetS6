
#include "level.h"

#include <stdlib.h>

#include "../utils/utils.h"

/**
 * @brief Create an empty level.
 * 
 * @return Level : The level.
 */
Level levelEmpty() {
    Level level;
    level.listeObjet = creerListeObjet();

	int i;
	for (i = 0; i < MATRICE_LEVEL_SIZE; i++)
		level.matriceSprite[i] = NULL;
    levelUpdateMatriceSprite(&level);

    return level;
}

SpriteData* creerSpriteData(char sprite, int color) {
    SpriteData* spriteD = malloc(sizeof(SpriteData));
	if (spriteD == NULL) {
		logs(L_DEBUG, "creerSpriteData | ERROR malloc spriteD");
		perror("Error while allocating memory in creerSpriteData\n");
		exit(EXIT_FAILURE);
	}

    spriteD->color = color;
    spriteD->specialChar = 0;
    spriteD->sprite = sprite;

    return spriteD;
}

SpriteData* creerSpriteDataS(chtype sprite, int color) {
    SpriteData* spriteD = malloc(sizeof(SpriteData));
	if (spriteD == NULL) {
		logs(L_DEBUG, "creerSpriteDataS | ERROR malloc spriteD");
		perror("Error while allocating memory in creerSpriteDataS\n");
		exit(EXIT_FAILURE);
	}
    spriteD->color = color;
    spriteD->specialChar = 1;
    spriteD->spSprite = sprite;

    return spriteD;
}

Level* levelCreer() {
    Level* level = malloc(sizeof(Level));
	if (level == NULL) {
		logs(L_DEBUG, "levelCreer | ERROR malloc level");
		perror("Error while allocating memory in levelCreer\n");
		exit(EXIT_FAILURE);
	}
    level->listeObjet = creerListeObjet();

    // Ajouter les murs de la map
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
            if (y == 0 || y == MATRICE_LEVEL_Y - 1 || x == 0 || x == MATRICE_LEVEL_X - 1) {
                Objet* bloc = creerBlock(x,y);
                listeAjouterObjet(level->listeObjet, bloc);
            }
            level->matriceSprite[y + x * MATRICE_LEVEL_Y] = NULL;
        }
    }

    levelUpdateMatriceSprite(level);
    return level;
}

void level_free(Level* level) {
    if (level == NULL) return;

    listeObjet_free(level->listeObjet, true);
    level->listeObjet = NULL;

    // free all the SpriteData
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
            SpriteData* sprite = level->matriceSprite[y+x*MATRICE_LEVEL_Y];
            if (sprite != NULL) {
                free(sprite);
                level->matriceSprite[y+x*MATRICE_LEVEL_Y] = NULL;
            }
        }
    }

    free(level);
}

SpriteData* emptySprite() {
    return creerSpriteData(' ', WHITE_COLOR);
}

void levelAjouterObjet(Level* level, Objet* objet) {
    listeAjouterObjet(level->listeObjet, objet);

    // update the matriceSprite
    levelUpdateMatriceSprite(level);
    // TODO update CollideMatrice
}

void levelSupprimerObjet(Level* level, Objet* objet) {
    listeSupprimerObjet(level->listeObjet, objet, true);

    // update the matriceSprite
    levelUpdateMatriceSprite(level);
    // TODO update CollideMatrice
}

ListeObjet* rechercherObjet(Level* level, short x, short y) {
    ListeObjet* liste = creerListeObjet();
    // parcourir les objets du level
    EltListe_o *elt = level->listeObjet->tete;
    while (elt != NULL) {
        Objet* objet = elt->objet;
        // verifier si x et y se trouve dans la zone delimité par les coordonnées de l'objet et sa taille
        if ( (x >= objet->x && x < objet->x + objet->xSize) && 
             (y <= objet->y && y > objet->y - objet->ySize)) {
            listeAjouterObjet(liste, objet);
        }
        elt = elt->suivant;
    }

    return liste;
}

void levelUpdateMatriceSprite(Level* level) {
    // clear the matrice
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
            SpriteData* sprite = level->matriceSprite[y+x*MATRICE_LEVEL_Y];
            if (sprite != NULL) {
                free(sprite);
            }
            level->matriceSprite[y+x*MATRICE_LEVEL_Y] = NULL;
        }
    }

    // parcourir les objets du level
    EltListe_o *elt = level->listeObjet->tete;
    while (elt != NULL) {
        char sprite;
        Objet* objet = elt->objet;
        int color, colorB;
        short x, y;
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

    // add the empty sprite
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
            if (level->matriceSprite[y+x*MATRICE_LEVEL_Y] == NULL) {
                level->matriceSprite[y+x*MATRICE_LEVEL_Y] = emptySprite();
            }
        }
    }
}

