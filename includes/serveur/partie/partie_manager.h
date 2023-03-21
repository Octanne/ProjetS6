#ifndef __PARTIE_MANAGER_H__
#define __PARTIE_MANAGER_H__

#include <stdbool.h>

#include "liste.h"
#include "data_update.h"

typedef struct {
    bool isStart;

    int nbPlayers;
    int maxPlayers;
    Liste players;
    Liste load_level;

    char map_file[255];
} Partie;

typedef struct {
    char name[255];
    int nbPlayers;
    int maxPlayers;
    int isStart;

    int portTCP;
} PartieStatutInfo;

typedef struct {
    Liste partieInfoListe;
} PartieManager;

void joinPartie();
void leavePartie();
void addPartie();

PartieListMessage listPartie(PartieManager *partieManager, int numPage);
MapListeMessage listMaps(PartieManager *partieManager, int numPage);

PartieManager partieManager_create();

#endif