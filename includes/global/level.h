
#ifndef LEVEL_H
#define LEVEL_H

#include <ncurses.h>

#include "liste.h"
#include "objet.h"

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
    Liste listeObjet;
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
Liste rechercherObjet(Level* level, short x, short y);

#endif

