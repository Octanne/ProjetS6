
#include "partie_manager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

#include "utils.h"
#include "constants.h"

void joinPartie() {

}

void leavePartie() {

}

void addPartie() {

}

PartieListeMessage listPartie(PartieManager *partieManager, int numPage) {
    logs(L_INFO, "PartieManager | listPartie numPage = %d", numPage);
    printf("PartieManager | listPartie numPage = %d \n", numPage);
    PartieListeMessage partieListeMessage;
    partieListeMessage.numPage = numPage;
    partieListeMessage.nbParties = 0;
    for (int i = 0; i < 4; i++) {
        PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, i + (numPage-1) * 4);
        if (partieInfo != NULL) {
            int numPartie = (i + (numPage-1) * 4);
            partieListeMessage.partieInfo[i].maxPlayers = partieInfo->maxPlayers;
            partieListeMessage.partieInfo[i].nbPlayers = partieInfo->nbPlayers;
            partieListeMessage.partieInfo[i].status = partieInfo->isStart;
            strcpy(partieListeMessage.partieInfo[i].name, partieInfo->name);
            partieListeMessage.partieInfo[i].set = true;
            partieListeMessage.partieInfo[i].numPartie = numPartie;
            printf("PartieManager | name = %s, num = %d\n", partieListeMessage.partieInfo[i].name, partieListeMessage.partieInfo[i].numPartie);
            // Incremente partieListeMessage.nbParties
            partieListeMessage.nbParties++;
        } else {
            partieListeMessage.partieInfo[i].set = false;
        }
    }

    return partieListeMessage;
}

MapListeMessage listMaps(PartieManager *partieManager, int numPage) {
    // Take the name of the maps in the folder maps
    // and return a MapListeMessage
    MapListeMessage mapListeMessage;
    mapListeMessage.numPage = numPage;
    mapListeMessage.nbMaps = 0;

    // Reads the file of a folder of relative paths "maps"
    DIR *dir;
    struct dirent *ent;
    char *path = "./maps/";
    char *extension = ".dat";

    // Open the directory
    if ((dir = opendir(path)) != NULL) {
        int i = 0;
        // Loop through all the files in the directory
        while ((ent = readdir(dir)) != NULL && i < (numPage) * 4) {
            // Check if the file has the extension ".dat"
            if (strstr(ent->d_name, extension) != NULL && i >= (numPage - 1) * 4) {
                // Add the name of the map to the list
                strcpy(mapListeMessage.mapInfo[i].name, ent->d_name);
                mapListeMessage.mapInfo[i].set = true;
                mapListeMessage.mapInfo[i].numMap = i;
                mapListeMessage.nbMaps++;
                i++;
            }
        }

        // Close the directory and check for errors
        if (closedir(dir) == -1) {
            // Print an error message if the directory cannot be closed
            printf("Cannot close directory: %s\n", path);
            exit(1);
        }
    } else {
        // Print an error message if the directory cannot be opened
        printf("Cannot open directory: %s\n", path);
        exit(1);
    }

    // Check if the number of maps is less than 4
    if (mapListeMessage.nbMaps < 4) {
        // Set the remaining maps to false
        for (int i = mapListeMessage.nbMaps; i < 4; i++) {
            mapListeMessage.mapInfo[i].set = false;
        }
    }

    return mapListeMessage;
}

PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap) {
    // TODO
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
