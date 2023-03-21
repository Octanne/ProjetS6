
#include "partie_manager.h"

#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "constants.h"

void joinPartie() {

}

void leavePartie() {

}

void addPartie() {

}

PartieListMessage listPartie(PartieManager *partieManager, int numPage) {
    logs(L_INFO, "PartieManager | listPartie numPage = %d", numPage);
    printf("PartieManager | listPartie numPage = %d \n", numPage);
    PartieListMessage partieListMessage;
    partieListMessage.numPage = numPage;
    partieListMessage.nbParties = 0;
    for (int i = 0; i < 4; i++) {
        PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, i + (numPage-1) * 4);
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

    // Mocks parties infos
    PartieStatutInfo partieInfo1;
    partieInfo1.maxPlayers = 4;
    partieInfo1.nbPlayers = 2;
    partieInfo1.isStart = false;
    strcpy(partieInfo1.name, "Partie 1");
    liste_add(&partieManager.partieInfoListe, &partieInfo1, TYPE_PARTIE_INFO);

    PartieStatutInfo partieInfo2;
    partieInfo2.maxPlayers = 4;
    partieInfo2.nbPlayers = 2;
    partieInfo2.isStart = false;
    strcpy(partieInfo2.name, "Partie 2");
    liste_add(&partieManager.partieInfoListe, &partieInfo2, TYPE_PARTIE_INFO);

    PartieStatutInfo partieInfo3;
    partieInfo3.maxPlayers = 4;
    partieInfo3.nbPlayers = 2;
    partieInfo3.isStart = false;
    strcpy(partieInfo3.name, "Partie 3");
    liste_add(&partieManager.partieInfoListe, &partieInfo3, TYPE_PARTIE_INFO);

    PartieStatutInfo partieInfo4;
    partieInfo4.maxPlayers = 4;
    partieInfo4.nbPlayers = 2;
    partieInfo4.isStart = false;
    strcpy(partieInfo4.name, "Partie 4");
    liste_add(&partieManager.partieInfoListe, &partieInfo4, TYPE_PARTIE_INFO);

    PartieStatutInfo partieInfo5;
    partieInfo5.maxPlayers = 4;
    partieInfo5.nbPlayers = 2;
    partieInfo5.isStart = false;
    strcpy(partieInfo5.name, "Partie 5");
    liste_add(&partieManager.partieInfoListe, &partieInfo5, TYPE_PARTIE_INFO);

    return partieManager;
}

void listMaps() {

}

