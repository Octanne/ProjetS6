#ifndef __PLAYER__
#define __PLAYER__

#include <stdbool.h>

typedef struct {
    char name[255];
    int life;
    int nbBombs;

    bool isAlive;
    bool isFreeze;
    bool isInvincible;

    bool key1;
    bool key2;
    bool key3;
    bool key4;

    int posX;
    int posY;
    int level;
} Player;

void player_free(Player *player);

#endif