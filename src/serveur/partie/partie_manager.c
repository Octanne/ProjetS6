
#include "partie_manager.h"

#include <string.h>

void joinPartie() {

}

void leavePartie() {

}

void addPartie() {

}

PartieListMessage listPartie(PartieManager *partieManager, int numPage) {
    PartieListMessage partieListMessage;
    partieListMessage.numPage = numPage;
    partieListMessage.nbParties = 0;
    for (int i = 0; i < 4; i++) {
        PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, i + numPage * 4);
        if (partieInfo != NULL) {
            partieListMessage.partieInfo[i].maxPlayers = partieInfo->maxPlayers;
            partieListMessage.partieInfo[i].nbPlayers = partieInfo->nbPlayers;
            partieListMessage.partieInfo[i].status = partieInfo->isStart;
            strcpy(partieListMessage.partieInfo[i].name, partieInfo->name);
            partieListMessage.partieInfo[i].set = true;

            // Incremente partieListMessage.nbParties
            partieListMessage.nbParties++;
        } else {
            partieListMessage.partieInfo[i].set = false;
        }
    }

    return partieListMessage;
}

PartieManager partieManager_create() {
    PartieManager partieManager;
    partieManager.partieInfoListe = liste_create();
    return partieManager;
}

void listMaps() {

}

