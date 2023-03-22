#ifndef __PARTIE_MANAGER_H__
#define __PARTIE_MANAGER_H__

#include <stdbool.h>

#include "liste.h"
#include "data_update.h"

typedef struct {
    char name[255];
    int nbPlayers;
    int maxPlayers;
    int isStart;

    int portTCP;
} PartieStatutInfo;

typedef struct {
    Liste players;
    Liste load_level;

    PartieStatutInfo *infosStatus;
} Partie;

typedef struct {
    Liste partieInfoListe;
} PartieManager;

PartieManager partieManager_create();

PartieListeMessage listPartie(PartieManager *partieManager, int numPage);
MapListeMessage listMaps(PartieManager *partieManager, int numPage);
PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap);

void joinPartie();
void leavePartie();



#endif