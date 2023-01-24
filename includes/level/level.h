#ifndef LEVEL_H
#define LEVEL_H

#include <stdlib.h>
#include <ncurses.h>

#include "../liste/liste.h"

typedef struct {
    char sprite;
    chtype spSprite;

    int specialChar;
    int color;
} SpriteData;

typedef struct {
    ListeObjet* listeObjet;
    SpriteData* matriceSprite[60*20];
    Objet* matriceCollide[60*20];

} Level;

SpriteData* creerSpriteData(char sprite, int color);
SpriteData* emptySprite();

Level* levelEmpty();

Level* levelCreer();
void level_free(Level* level);
void levelAjouterObjet(Level* level, Objet* objet);
void levelSupprimerObjet(Level* level, Objet* objet);
void levelUpdateMatriceSprite(Level* level);
ListeObjet* rechercherObjet(Level* level, int x, int y);

#endif