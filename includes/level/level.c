#include "level.h"

#include <stdlib.h>

#include "../constants.h"

Objet** levelCreateSaveArray(Level* level) {
    Objet** saveMatrice = malloc(sizeof(Objet*) * level->listeObjet->taille);

    // Parcourir la liste des objets
    EltListe_o* obj = level->listeObjet->tete;
    int i = 0;
    while (obj != NULL) {
        // Ajouter l'objet à la matrice
        saveMatrice[i] = obj->objet;
        i++;
    }

    return saveMatrice;
}

Level* levelLoadFromSaveArray(Objet** saveMatrice) {
    Level* level = levelCreer();
    int numObjet = sizeof(saveMatrice) / sizeof(Objet);
    // Parcourir la matrice
    int i = 0;
    for (i = 0; i < numObjet; i++) {
        // Ajouter l'objet à la liste
        listeAjouterObjet(level->listeObjet, saveMatrice[i]);
    }

    return level;
}

SpriteData* creerSpriteData(char sprite, int color) {
    SpriteData* spriteD = malloc(sizeof(SpriteData));
    spriteD->color = color;
    spriteD->specialChar = 0;
    spriteD->sprite = sprite;

    return spriteD;
}

SpriteData* creerSpriteDataS(chtype sprite, int color) {
    SpriteData* spriteD = malloc(sizeof(SpriteData));
    spriteD->color = color;
    spriteD->specialChar = 1;
    spriteD->spSprite = sprite;

    return spriteD;
}

Level* levelCreer() {
    Level* level = malloc(sizeof(Level));
    level->listeObjet = creerListeObjet();

    // Ajouter les murs de la map
    int y, x;
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            if (y == 0 || y == 19 || x == 0 || x == 59) {
                Objet* bloc = creerBlock(x,y);
                listeAjouterObjet(level->listeObjet,bloc);
            }
            level->matriceSprite[y+x*20] = NULL;
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
    int y, x;
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            SpriteData* sprite = level->matriceSprite[y+x*20];
            if (sprite != NULL) {
                free(sprite);
                level->matriceSprite[y+x*20] = NULL;
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

ListeObjet* rechercherObjet(Level* level, int x, int y) {
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
    int y, x;
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            SpriteData* sprite = level->matriceSprite[y+x*20];
            if (sprite != NULL) {
                free(sprite);
            }
            level->matriceSprite[y+x*20] = NULL;
        }
    }

    // parcourir les objets du level
    EltListe_o *elt = level->listeObjet->tete;
    while (elt != NULL) {
        char sprite;
        Objet* objet = elt->objet;
        int color, colorB;
        int x, y;
        switch (objet->type) {
            case BLOCK_ID :
                sprite = ' ';
                level->matriceSprite[objet->y + objet->x*20] = creerSpriteData(sprite, LBLUE_BLOCK);
                break;
            case HEART_ID :
                sprite = 'V';
                level->matriceSprite[objet->y + objet->x*20] = creerSpriteData(sprite, RED_COLOR);
                break;
            case TRAP_ID :
                sprite = '#';
                level->matriceSprite[objet->y + objet->x*20] = creerSpriteData(sprite, LBLUE_BLOCK);
                break;
            case BOMB_ID :
                sprite = 'o';
                level->matriceSprite[objet->y + objet->x*20] = creerSpriteData(sprite, WHITE_COLOR);
                break;
            case GATE_ID :
                color = WHITE_COLOR;
                if (objet->objet.gate.numgate == 0) color = PURPLE_COLOR;
                else if (objet->objet.gate.numgate == 1) color = GREEN_COLOR;
                else if (objet->objet.gate.numgate == 2) color = YELLOW_COLOR;
                else if (objet->objet.gate.numgate == 3) color = LBLUE_COLOR;
                level->matriceSprite[objet->y + objet->x*20] = creerSpriteDataS(ACS_PLUS, color);
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
                level->matriceSprite[objet->y-1 + objet->x*20] = creerSpriteData(' ', colorB);;
                // Sprite #2
                level->matriceSprite[objet->y + objet->x*20] = creerSpriteDataS(ACS_LLCORNER, color);
                break; 
            case DOOR_ID:
                for (y = 0; y < objet->ySize; y++) {
                    for (x = 0; x < objet->xSize; x++) {
                        // Lead 0
                        if (y == 3 && x < 2) {
                            int numDoor = objet->objet.door.numdoor;
                            char numDoorChar[4];
                            sprintf(numDoorChar, "%02i", numDoor);
                            if (x == 0) level->matriceSprite[(objet->y-y) + (objet->x+x)*20] = creerSpriteData(numDoorChar[0], WHITE_COLOR);
                            if (x == 1) level->matriceSprite[(objet->y-y) + (objet->x+x)*20] = creerSpriteData(numDoorChar[1], WHITE_COLOR);
                        } else level->matriceSprite[(objet->y-y) + (objet->x+x)*20] = creerSpriteData(' ', GREEN_BLOCK);
                    }
                }
                break;
            case EXIT_ID :
                for (y = 0; y < objet->ySize; y++) {
                    for (x = 0; x < objet->xSize; x++) {
                        level->matriceSprite[(objet->y-y) + (objet->x+x)*20] = creerSpriteData(' ', YELLOW_BLOCK);
                    }
                }
                break;
            case START_ID :
                for (y = 0; y < objet->ySize; y++) {
                    for (x = 0; x < objet->xSize; x++) {
                        level->matriceSprite[(objet->y-y) + (objet->x+x)*20] = creerSpriteData(' ', PURPLE_BLOCK);
                    }
                }
                break;
            case LADDER_ID :
                level->matriceSprite[objet->y + (objet->x)*20] = creerSpriteDataS(ACS_LTEE, YELLOW_COLOR);
                level->matriceSprite[objet->y + (objet->x+1)*20] = creerSpriteDataS(ACS_HLINE, YELLOW_COLOR);
                level->matriceSprite[objet->y + (objet->x+2)*20] = creerSpriteDataS(ACS_RTEE, YELLOW_COLOR);
                break;
            case PROBE_ID :
                level->matriceSprite[objet->y-1 + (objet->x)*20] = creerSpriteDataS(ACS_LTEE, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+1)*20] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+2)*20] = creerSpriteDataS(ACS_RTEE, WHITE_COLOR);

                level->matriceSprite[objet->y + (objet->x)*20] = creerSpriteDataS(ACS_LLCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+1)*20] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+2)*20] = creerSpriteDataS(ACS_LRCORNER, WHITE_COLOR);
                break;
            case ROBOT_ID :
                level->matriceSprite[objet->y-3 + (objet->x)*20] = creerSpriteDataS(ACS_ULCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y-3 + (objet->x+1)*20] = creerSpriteDataS(ACS_BTEE, WHITE_COLOR);
                level->matriceSprite[objet->y-3 + (objet->x+2)*20] = creerSpriteDataS(ACS_URCORNER, WHITE_COLOR);

                level->matriceSprite[objet->y-2 + (objet->x)*20] = creerSpriteDataS(ACS_LLCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y-2 + (objet->x+1)*20] = creerSpriteDataS(ACS_TTEE, WHITE_COLOR);
                level->matriceSprite[objet->y-2 + (objet->x+2)*20] = creerSpriteDataS(ACS_LRCORNER, WHITE_COLOR);

                level->matriceSprite[objet->y-1 + (objet->x)*20] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+1)*20] = creerSpriteDataS(ACS_PLUS, WHITE_COLOR);
                level->matriceSprite[objet->y-1 + (objet->x+2)*20] = creerSpriteDataS(ACS_HLINE, WHITE_COLOR);

                level->matriceSprite[objet->y + (objet->x)*20] = creerSpriteDataS(ACS_ULCORNER, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+1)*20] = creerSpriteDataS(ACS_BTEE, WHITE_COLOR);
                level->matriceSprite[objet->y + (objet->x+2)*20] = creerSpriteDataS(ACS_URCORNER, WHITE_COLOR);
                break;
            case PLAYER_ID :
                color = objet->objet.player.color;
                // head
                if (objet->objet.player.orientation == LEFT_ORIENTATION) {
                    level->matriceSprite[objet->y-3 + (objet->x+1)*20] = creerSpriteData(' ', RED_BLOCK);
                    level->matriceSprite[objet->y-3 + (objet->x)*20] = creerSpriteData('-', RED_BLOCK);
                } else {
                    level->matriceSprite[objet->y-3 + (objet->x+1)*20] = creerSpriteData(' ', RED_BLOCK);
                    level->matriceSprite[objet->y-3 + (objet->x+2)*20] = creerSpriteData('-', RED_BLOCK);
                }

                // body
                level->matriceSprite[objet->y-2 + (objet->x)*20] = creerSpriteDataS(ACS_HLINE, color);
                level->matriceSprite[objet->y-2 + (objet->x+1)*20] = creerSpriteDataS(ACS_PLUS, color);
                level->matriceSprite[objet->y-2 + (objet->x+2)*20] = creerSpriteDataS(ACS_HLINE, color);

                level->matriceSprite[objet->y-1 + (objet->x)*20] = creerSpriteDataS(ACS_ULCORNER, color);
                level->matriceSprite[objet->y-1 + (objet->x+1)*20] = creerSpriteDataS(ACS_BTEE, color);
                level->matriceSprite[objet->y-1 + (objet->x+2)*20] = creerSpriteDataS(ACS_URCORNER, color);

                level->matriceSprite[objet->y + (objet->x)*20] = creerSpriteDataS(ACS_VLINE, color);
                level->matriceSprite[objet->y + (objet->x+2)*20] = creerSpriteDataS(ACS_VLINE, color);
                break;
        }
        elt = elt->suivant;
    }

    // add the empty sprite
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            if (level->matriceSprite[y+x*20] == NULL) {
                level->matriceSprite[y+x*20] = emptySprite();
            }
        }
    }
}