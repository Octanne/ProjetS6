
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

#endif // __DATA_UPDATE_H__

