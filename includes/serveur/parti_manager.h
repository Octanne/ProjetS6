#ifndef __PARTIE_MANAGER_H__
#define __PARTIE_MANAGER_H__

#include <stdbool.h>
#include "liste.h"

typedef struct {
    bool isStart;

    int nbPlayers;
    int maxPlayers;
    ListePlayer players;
} Partie;

void joinPartie();
void leavePartie();

void listPartie();
void addPartie();
void removePartie();

#endif