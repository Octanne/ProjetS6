
#include "partie_manager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

#include "utils.h"
#include "constants.h"

// ### UDP ###
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
            strcpy(partieListeMessage.partieInfo[i].name, partieInfo->map);
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

Liste getMapsListe() {
    // Reads the file of a folder of relative paths "maps"
    DIR *dir;
    struct dirent *ent;
    char *path = "./maps/";
    char *extension = ".dat";
    Liste mapListe = liste_create();

    // Open the directory
    if ((dir = opendir(path)) != NULL) {
        int i = 0;
        // Loop through all the files in the directory
        while ((ent = readdir(dir)) != NULL) {
            // Check if the file has the extension ".dat"
            if (strstr(ent->d_name, extension) != NULL) {
                MapInfo *mapInfo = malloc(sizeof(MapInfo));
                strcpy(mapInfo->name, ent->d_name);
                mapInfo->numMap = i;
                mapInfo->set = true;
                liste_add(&mapListe, mapInfo, TYPE_MAP_INFO);
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

    return mapListe;
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

PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap, struct sockaddr_in clientAddr) {
    PartieCreateMessage partieCreateMessage;
    partieCreateMessage.success = false;
    partieCreateMessage.serverPortTCP = -1;
    
    // Logs the request data
    logs(L_INFO, "PartieManager | createPartie | maxPlayers = %d, numMap = %d", maxPlayers, numMap);
    printf("PartieManager | createPartie | maxPlayers = %d, numMap = %d\n", maxPlayers, numMap);

    // Check the map 
    Liste mapListe = getMapsListe();
    MapInfo *mapInfo = (MapInfo*)liste_get(&mapListe, numMap);
    logs(L_INFO, "PartieManager | createPartie | mapInfo = %s", mapInfo->name);
    if (mapInfo != NULL && maxPlayers > 0) {
        PartieStatutInfo *partieInfo = malloc(sizeof(PartieStatutInfo));
        partieInfo->maxPlayers = maxPlayers;
        partieInfo->nbPlayers = 1;
        partieInfo->isStart = false;
        partieInfo->pid_partie_process = -1;
        partieInfo->playersInWait = liste_create();
        strcpy(partieInfo->map, mapInfo->name);

        if (maxPlayers == 1) {
            // on démarre le TCPServer
            if (startPartieProcessus(partieManager, partieInfo) == -1) {
                // Si le TCPServer ne démarre pas on crash
                logs(L_INFO, "PartieManager | TCPServer not started");
                printf("PartieManager | TCPServer not started");
                exit(EXIT_FAILURE);
            }

            // On récupère le port du TCPServer
            partieCreateMessage.serverPortTCP = partieInfo->portTCP;
            logs(L_INFO, "PartieManager | TCPServer started on port %d", partieInfo->portTCP);
            printf("PartieManager | TCPServer started on port %d\n", partieInfo->portTCP);
        } else {
            // Add the client to the wait list
            struct sockaddr_in *clientAddrCopy = malloc(sizeof(struct sockaddr_in));
            memcpy(clientAddrCopy, &clientAddr, sizeof(struct sockaddr_in));
            liste_add(&partieInfo->playersInWait, clientAddrCopy, TYPE_SOCKADDR_IN);
            logs(L_INFO, "PartieManager | Client added to the wait list");
            printf("PartieManager | Client added to the wait list\n");
        }

        // Add the partie to the list
        liste_add(&partieManager->partieInfoListe, partieInfo, TYPE_PARTIE_INFO);
        partieCreateMessage.success = true;
        partieCreateMessage.numPartie = partieManager->partieInfoListe.taille - 1;
        logs(L_INFO, "PartieManager | Partie created");
        printf("PartieManager | Partie created | numPartie = %d\n", partieCreateMessage.numPartie);
    }
    liste_free(&mapListe, true);

    return partieCreateMessage;
}

PartieJoinLeaveWaitMessage waitListePartie(PartieManager *partieManager, int numPartie, bool waitState, struct sockaddr_in clientAddr) {
    PartieJoinLeaveWaitMessage partieJoinLeaveWaitMessage;
    partieJoinLeaveWaitMessage.takeInAccount = false;
    partieJoinLeaveWaitMessage.portTCP = -1;

    // Check if the partie exists
    PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, numPartie);
    if (partieInfo != NULL) {
        // Check if he want to leave or join the partie
        if (waitState) {
            // Check if the partie is not full
            if (partieInfo->nbPlayers < partieInfo->maxPlayers) {
                // Check if the partie is not started
                if (!partieInfo->isStart) {
                    // Add the client to the wait list
                    struct sockaddr_in *clientAddrCopy = malloc(sizeof(struct sockaddr_in));
                    memcpy(clientAddrCopy, &clientAddr, sizeof(struct sockaddr_in));
                    liste_add(&partieInfo->playersInWait, clientAddrCopy, TYPE_SOCKADDR_IN);
                    partieJoinLeaveWaitMessage.takeInAccount = true;
                } else {
                    // The partie is started
                    partieJoinLeaveWaitMessage.portTCP = partieInfo->portTCP;
                    partieJoinLeaveWaitMessage.takeInAccount = true;
                }
                // Update the number of players
                partieInfo->nbPlayers++;
            }
        } else {
            // Remove the client from the wait list
            // Search the client in the wait list
            for (int i = 0; i < partieInfo->playersInWait.taille; i++) {
                struct sockaddr_in *clientAddrInListe = (struct sockaddr_in*)liste_get(&partieInfo->playersInWait, i);
                if (clientAddrInListe->sin_addr.s_addr == clientAddr.sin_addr.s_addr && clientAddrInListe->sin_port == clientAddr.sin_port) {
                    // Remove the client from the wait list
                    liste_remove(&partieInfo->playersInWait, clientAddrInListe, true);
                    partieJoinLeaveWaitMessage.takeInAccount = true;
                    // Update the number of players
                    partieInfo->nbPlayers--;
                    break;
                }
            }
        }
    }

    return partieJoinLeaveWaitMessage;
}

int startPartieProcessus(PartieManager *partieManager, PartieStatutInfo *partieInfo) {
    // Create the socket and process TODO
    return 0;
}

PartieManager partieManager_create() {
    PartieManager partieManager;
    partieManager.partieInfoListe = liste_create();

    return partieManager;
}

