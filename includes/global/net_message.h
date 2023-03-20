
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#define data_size 2048

#define NET_REQ_PING           0
#define NET_REQ_PARTIE_LIST    1
#define NET_REQ_PARTIE_INFO    2
#define NET_REQ_PARTIE_JOIN    3
#define NET_REQ_PARTIE_LEAVE   4
#define NET_REQ_PARTIE_CREATE  5

#define NET_TIMEOUT 2 // 5 seconds
#define NET_MAX_TRIES 3

#include "player.h"
#include "constants.h"
#include "objet.h"

typedef struct {
    Player player;
    int nbLevelElmt;
    char levelBytes[sizeof(Objet)*(MATRICE_LEVEL_SIZE+MAX_PLAYERS)];
} DataUpdateGame;

typedef struct {
    // TODO add needed
} DataUpdateMenu;

typedef struct {
    char playerID[255];
} DataInputPlayer;

typedef struct {
    int type;
    int size;
    union {
        DataUpdateGame updateGame;
        DataUpdateMenu updateMenu;
        DataInputPlayer inputPlayer;
    };
} NetMessage;

#endif

