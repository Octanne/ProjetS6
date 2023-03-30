
#ifndef __PLAYER__
#define __PLAYER__

#include <stdbool.h>
#include <pthread.h>

#include "objet.h"

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

	int level;
	int numPlayer;

	Objet *obj;
	pthread_mutex_t mutex;
} Player;

void player_free(Player *player);

#endif

