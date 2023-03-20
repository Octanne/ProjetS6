
#ifndef LEVEL_H
#define LEVEL_H

#include <ncurses.h>

#include "liste.h"
#include "objet.h"
#include "constants.h"

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
Level convert_bytes_to_level(char* bytes, size_t size);
char* convert_level_to_bytes(Level level, size_t* size);

#endif

