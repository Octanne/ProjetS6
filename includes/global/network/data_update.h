
#ifndef __DATA_UPDATE_H__
#define __DATA_UPDATE_H__

#include <stddef.h>

#include "constants.h"
#include "player.h"
#include "objet.h"

typedef struct {
    Player player;
    size_t sizeLevel;
    char levelBytes[sizeof(Objet)*(MATRICE_LEVEL_SIZE+MAX_PLAYERS)];
} DataUpdateGame;

typedef struct {
    // TODO add needed
} DataUpdateMenu;

typedef struct {
    char playerID[255];
} DataInputPlayer;

typedef struct {
    char text[255];
    int line;
    int color;
} DataTextInfoGUI;

typedef struct {
    int type;
    union {
        DataUpdateGame updateGame;
        DataUpdateMenu updateMenu;
        DataInputPlayer inputPlayer;
        DataTextInfoGUI textInfoGUI;
    };
} DataChangeMessage;

#endif // __DATA_UPDATE_H__

