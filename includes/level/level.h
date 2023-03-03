
#ifndef LEVEL_H
#define LEVEL_H

#include "../liste/liste.h"
#include <ncurses.h>

#define MATRICE_LEVEL_X 60
#define MATRICE_LEVEL_Y 20
#define MATRICE_LEVEL_SIZE 1200

typedef struct {
    char sprite;
    chtype spSprite;

    int specialChar;
    int color;
} SpriteData;

typedef struct {
    ListeObjet listeObjet;
    SpriteData matriceSprite[MATRICE_LEVEL_SIZE];
    Objet* matriceCollide[MATRICE_LEVEL_SIZE];
} Level;

SpriteData creerSpriteData(char sprite, int color);
SpriteData emptySprite();

Level levelEmpty();

Level levelCreer();
void level_free(Level* level);
void levelAjouterObjet(Level* level, Objet* objet);
void levelSupprimerObjet(Level* level, Objet* objet);
void levelUpdateMatriceSprite(Level* level);
ListeObjet rechercherObjet(Level* level, short x, short y);

#endif

