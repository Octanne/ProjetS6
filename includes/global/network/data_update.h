
#ifndef __DATA_UPDATE_H__
#define __DATA_UPDATE_H__

#include <stddef.h>

#include "constants.h"
#include "player.h"
#include "objet.h"

/* UDP messages */

// ######## //

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
    int numPartie;
    int status;

    bool set;
} PartieInfo;

typedef struct {
    int numPage;
    int nbParties;
    PartieInfo partieInfo[4];
} PartieListeMessage;

// ######## //

// ######## //

typedef struct {
    int maxPlayers;
    int numMap;

    bool success;
    int numPartie;
    int serverPortTCP;
} PartieCreateMessage;

// ######## //

typedef struct {
    int numPartie;
    bool waitState;

    bool takeInAccount;
    int portTCP;
} PartieJoinLeaveWaitMessage;

// ######## //

/* TCP messages */

typedef struct {
    Player player;
    size_t sizeLevel;
    char levelBytes[sizeof(Objet) * (MATRICE_LEVEL_SIZE + MAX_PLAYERS)];
} DataUpdateGame;

typedef struct {
    int keyPress;
} DataInputPlayer;

typedef struct {
    char text[255];
    int line;
    int color;
} DataTextInfoGUI;

#endif // __DATA_UPDATE_H__

