
#ifndef __DATA_UPDATE_H__
#define __DATA_UPDATE_H__

#include <stddef.h>

#include "constants.h"
#include "player.h"
#include "objet.h"

typedef struct {
    char name[36];

    int numMap;
    bool set;
} MapInfo;

typedef struct {
    int numPage;
    int nbMaps;
    MapInfo mapInfo[4];
} MapListeMessage;

// ######## //

typedef struct {
    char name[36];
    int nbPlayers;
    int maxPlayers;
    int status;

    int numPartie;
    bool set;
} PartieInfo;

typedef struct {
    int numPage;
    int nbParties;
    PartieInfo partieInfo[4];
} PartieListMessage;

// ######## //

typedef struct {

} DataUpdateMenu;

typedef struct {
    Player player;
    size_t sizeLevel;
    char levelBytes[sizeof(Objet)*(MATRICE_LEVEL_SIZE+MAX_PLAYERS)];
} DataUpdateGame;

typedef struct {
    char playerID[255];
} DataInputPlayer;

typedef struct {
    char text[255];
    int line;
    int color;
} DataTextInfoGUI;

#endif // __DATA_UPDATE_H__

