
#ifndef __PARTIE_MANAGER_H__
#define __PARTIE_MANAGER_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "liste.h"
#include "data_update.h"

typedef struct {
    char map[255];
    int isStart;

    int portTCP;
    int nbPlayers;
    int maxPlayers;

    Liste playersInWait;
    bool tcpStart;
    int pid_partie_process;
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


// ### UDP ###
PartieListeMessage listPartie(PartieManager *partieManager, int numPage);
MapListeMessage listMaps(PartieManager *partieManager, int numPage);
PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap, struct sockaddr_in clientAddr);
PartieJoinLeaveWaitMessage waitListePartie(PartieManager *partieManager, int numPartie, bool waitState, struct sockaddr_in clientAddr);

// ### TCP ###
int startPartieProcessus(PartieManager *partieManager, PartieStatutInfo *partieInfo);

void joinPartieTCP();
void leavePartieTCP();

#endif

