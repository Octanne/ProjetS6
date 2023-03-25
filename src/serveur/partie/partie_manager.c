
#include "partie_manager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

#include "utils.h"
#include "constants.h"

#define NUM_PARTIES_PAR_PAGE 4

/**
 * @brief Create a Partie Manager object linked to the udp socket
 * 
 * @param udpSocket		UDPSocketData of the server
 * 
 * @return PartieManager
*/
PartieManager partieManager_create(UDPSocketData udpSocket) {
	PartieManager partieManager;
	partieManager.partieInfoListe = liste_create(true);
	partieManager.udpSocket = udpSocket;
	return partieManager;
}


// ### UDP ###
/**
 * @brief Create a List of games on the server depending on the page number
 * Used to send the list to the client via UDP for the game selection
 * 
 * @param partieManager	Pointer to the game manager
 * @param numPage		Page number
 * 
 * @return PartieListeMessage
*/
PartieListeMessage listPartie(PartieManager *partieManager, int numPage) {

	// Initialisation
	PartieListeMessage partieListeMessage;
	partieListeMessage.numPage = numPage;
	partieListeMessage.nbParties = 0;

	// Loop through all the games in the current page
	for (int i = 0; i < NUM_PARTIES_PAR_PAGE; i++) {

		// Get the game
		int numPartie = i + (numPage-1) * NUM_PARTIES_PAR_PAGE;
		PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, numPartie);

		// If the game exists, add it to the list
		if (partieInfo != NULL) {
			partieListeMessage.partieInfo[i].numPartie = numPartie;
			partieListeMessage.partieInfo[i].set = true;
			partieListeMessage.partieInfo[i].maxPlayers = partieInfo->maxPlayers;
			partieListeMessage.partieInfo[i].nbPlayers = partieInfo->nbPlayers;
			partieListeMessage.partieInfo[i].status = partieInfo->isStart;
			strcpy(partieListeMessage.partieInfo[i].name, partieInfo->map);
			printf("PartieManager | Map = %s, numPartie = %d, MaxPlayers = %d, Players = %d\n", partieListeMessage.partieInfo[i].name, 
				partieListeMessage.partieInfo[i].numPartie, partieListeMessage.partieInfo[i].maxPlayers, partieListeMessage.partieInfo[i].nbPlayers);
			partieListeMessage.nbParties++;
		} else {
			partieListeMessage.partieInfo[i].set = false;
		}
	}

	return partieListeMessage;
}

/**
 * @brief Create a queue of maps
 * 
 * @return Liste
*/
Liste getMapsListe() {
	// Reads the file of a folder of relative paths "maps"
	DIR *dir;
	struct dirent *ent;
	char *path = "./maps/";
	char *extension = ".dat";
	Liste mapListe = liste_create(true);

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

/**
 * @brief Create a list of maps on the game depending on the page number
 * 
 * @param partieManager	Pointer to the game manager
 * @param numPage		Page number
 * 
 * @return MapListeMessage
*/
MapListeMessage listMaps(PartieManager *partieManager, int numPage) {
	Liste mapListe = getMapsListe();

	MapListeMessage mapListeMessage;
	mapListeMessage.numPage = numPage;
	mapListeMessage.nbMaps = 0;

	// Loop through all the maps in the current page
	for (int i = 0; i < 4; i++) {

		// Get the map info
		int numMap = i + (numPage-1) * 4;
		MapInfo *mapInfo = (MapInfo*)liste_get(&mapListe, numMap);

		// If the map exists, add it to the list
		if (mapInfo != NULL) {
			mapListeMessage.mapInfo[i].numMap = numMap;
			mapListeMessage.mapInfo[i].set = true;
			strcpy(mapListeMessage.mapInfo[i].name, mapInfo->name);
			mapListeMessage.nbMaps++;
		} else {
			mapListeMessage.mapInfo[i].set = false;
		}
	}

	liste_free(&mapListe, TYPE_MAP_INFO);

	return mapListeMessage;
}

/**
 * @brief Create a structure to use to create a game
 * 
 * @param partieManager	Pointer to the game manager
 * @param maxPlayers	Maximum number of players
 * @param numMap		Number of the map
 * @param clientAddr	Address of the client
 * 
 * @return PartieCreateMessage
*/
PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap, struct sockaddr_in clientAddr) {
	PartieCreateMessage partieCreateMessage;
	partieCreateMessage.maxPlayers = maxPlayers;
	partieCreateMessage.numMap = numMap;
	partieCreateMessage.numPartie = -1;
	partieCreateMessage.success = false;
	partieCreateMessage.serverPortTCP = -1;
	
	// Logs the request data
	logs(L_INFO, "PartieManager | createPartie | maxPlayers = %d, numMap = %d", maxPlayers, numMap);
	printf("PartieManager | createPartie | maxPlayers = %d, numMap = %d\n", maxPlayers, numMap);

	// Check the maxPlayers
	if (maxPlayers <= 0) {
		logs(L_DEBUG, "PartieManager | createPartie | maxPlayers <= 0");
		printf("PartieManager | createPartie | maxPlayers <= 0 \n");
		return partieCreateMessage;
	}

	// Check the map 
	Liste mapListe = getMapsListe();
	MapInfo *mapInfo = (MapInfo*)liste_get(&mapListe, numMap);

	if (mapInfo == NULL) {
		logs(L_DEBUG, "PartieManager | createPartie | mapInfo == NULL");
		printf("PartieManager | createPartie | mapInfo == NULL \n");
		return partieCreateMessage;
	}

	// Create a status structure for the game
	PartieStatutInfo *partieInfo = malloc(sizeof(PartieStatutInfo));
	strcpy(partieInfo->map, mapInfo->name);
	partieInfo->maxPlayers = maxPlayers;
	partieInfo->nbPlayers = 1;
	partieInfo->isStart = false;
	partieInfo->portTCP = -1;
	partieInfo->tcpStart = false;
	partieInfo->pid_partie_process = -1;
	partieInfo->playersInWait = liste_create(false);

	// Free the mapListe
	liste_free(&mapListe, true);

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
	logs(L_INFO, "PartieManager | Partie created | numPartie = %d", partieCreateMessage.numPartie);
	printf("PartieManager | Partie created | numPartie = %d\n", partieCreateMessage.numPartie);

	return partieCreateMessage;
}

/**
 * @brief Manage requests about joining or leaving a game
 * 
 * @param partieManager	Pointer to the game manager
 * @param numPartie		Game number
 * @param waitState		True if the client want to join the game, false if he want to leave
 * @param clientAddr	Address of the client
 * 
 * @return PartieJoinLeaveWaitMessage
*/
PartieJoinLeaveWaitMessage waitListePartie(PartieManager *partieManager, int numPartie, bool waitState, struct sockaddr_in clientAddr) {

	// Init the return message
	PartieJoinLeaveWaitMessage partieJoinLeaveWaitMessage;
	partieJoinLeaveWaitMessage.numPartie = numPartie;
	partieJoinLeaveWaitMessage.waitState = waitState;
	partieJoinLeaveWaitMessage.takeInAccount = false;
	partieJoinLeaveWaitMessage.portTCP = -1;

	// Check if the partie exists
	PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, numPartie);

	if (partieInfo == NULL) {
		return partieJoinLeaveWaitMessage;
	}

	// Check if he want to leave or join the partie
	if (waitState) {
		// Check if the partie is not full
		if (partieInfo->nbPlayers >= partieInfo->maxPlayers) {
			return partieJoinLeaveWaitMessage;
		}

		// Update the number of players
		partieInfo->nbPlayers++;

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

			return partieJoinLeaveWaitMessage;
		}

		// Check if the partie can start now
		if (partieInfo->nbPlayers == partieInfo->maxPlayers) {
			// Start the partie
			if (startPartieProcessus(partieManager, partieInfo) == -1) {
				// If the TCPServer cannot start, we crash
				logs(L_INFO, "PartieManager | TCPServer not started");
				printf("PartieManager | TCPServer not started");
				exit(EXIT_FAILURE);
			}

			// Get the port of the TCPServer
			partieJoinLeaveWaitMessage.portTCP = partieInfo->portTCP;
			logs(L_INFO, "PartieManager | TCPServer started on port %d", partieInfo->portTCP);
			printf("PartieManager | TCPServer started on port %d\n", partieInfo->portTCP);
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

	return partieJoinLeaveWaitMessage;
}

/**
 * @brief Start the partie processus
 * - Create the socket with port 0 (listen on a random port)
 * - Fills the PartieStatutInfo with the port that will be send to clients by another function
 * 
 * @param partieManager	PartieManager
 * @param partieInfo	PartieStatutInfo
 * 
 * @return int (0 if success, -1 if error)
*/
int startPartieProcessus(PartieManager *partieManager, PartieStatutInfo *partieInfo) {
	// Create the socket
	int socketTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketTCP == -1) {
		logs(L_DEBUG, "PartieManager | startPartieProcessus | socketTCP == -1");
		return -1;
	}

	// Create the socket address
	struct sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(0);
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind the socket
	if (bind(socketTCP, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1) {
		logs(L_DEBUG, "PartieManager | startPartieProcessus | bind == -1");
		return -1;
	}

	// Get the port of the socket
	struct sockaddr_in socketAddrOut;
	socklen_t socketAddrOutSize = sizeof(socketAddrOut);
	if (getsockname(socketTCP, (struct sockaddr*)&socketAddrOut, &socketAddrOutSize) == -1) {
		logs(L_DEBUG, "PartieManager | startPartieProcessus | getsockname == -1");
		return -1;
	}

	// Fill the PartieStatutInfo
	partieInfo->portTCP = ntohs(socketAddrOut.sin_port);

	// Create the processus that will handle the socket
	pid_t pid = fork();
	if (pid == 0) {
		// Child processus (partie processus)
		// Close the UDP socket
		if (close(partieManager->udpSocket.sockfd) == -1) {
			logs(L_DEBUG, "PartieManager | startPartieProcessus | close == -1");
			return -1;
		}

		// Launch the TCPServer manager with a copy of PartieStatutInfo (copy because this is a child process)
		partieProcessusManager(socketTCP, *partieInfo);
	}
	
	// Parent processus (PartieManager processus)
	// Close the TCP socket (the child processus will handle it)
	if (close(socketTCP) == -1) {
		logs(L_DEBUG, "PartieManager | startPartieProcessus | close == -1");
		return -1;
	}

	return 0;
}

/**
 * @brief Partie processus manager
 * - Wait for clients to connect to the TCPServer
 * - Launch a thread for each client
 * 
 * @param sockedTCP		Socket TCP file descriptor (binded)
 * @param partieInfo	PartieStatutInfo containing informations about the game
 * 
 * @return void
*/
void partieProcessusManager(int sockedTCP, PartieStatutInfo partieInfo) {

	// Listen on the socket
	if (listen(sockedTCP, 10) == -1) {
		logs(L_DEBUG, "PartieManager | partieProcessusManager | listen == -1");
		exit(EXIT_FAILURE);
	}

	// Create threadsSharedMemory and create thread list depending on the number of players
	threadsSharedMemory th_shared_memory;
	th_shared_memory.threads = malloc(sizeof(pthread_t) * partieInfo.nbPlayers);
	th_shared_memory.thread_states = malloc(sizeof(int) * partieInfo.nbPlayers);
	th_shared_memory.nbThreads = partieInfo.nbPlayers;
	memset(th_shared_memory.thread_states, 0, sizeof(int) * partieInfo.nbPlayers);

	// Create threadTCPArgs list to pass arguments to threads
	threadTCPArgs *th_args_list = malloc(sizeof(threadTCPArgs) * partieInfo.nbPlayers);

	// Wait for clients to connect
	int i;
	for (i = 0; i < partieInfo.nbPlayers; i++) {

		// Accept a client
		struct sockaddr_in clientAddr;
		socklen_t clientAddrSize = sizeof(clientAddr);
		int clientSocket = accept(sockedTCP, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (clientSocket == -1) {
			logs(L_DEBUG, "PartieManager | partieProcessusManager | accept == -1");
			exit(EXIT_FAILURE);
		}

		// Link arguments to the thread
		th_args_list[i].threadId = i;
		th_args_list[i].clientSocket = clientSocket;
		th_args_list[i].sharedMemory = &th_shared_memory;

		// Create the thread
		int status = pthread_create(&th_shared_memory.threads[i], NULL, partieThreadTCP, &th_args_list[i]);
		if (status != 0) {
			logs(L_DEBUG, "PartieManager | partieProcessusManager | pthread_create != 0");
			exit(EXIT_FAILURE);
		}
	}

	// Wait for all threads to finish
	for (i = 0; i < partieInfo.nbPlayers; i++) {
		pthread_join(th_shared_memory.threads[i], NULL);
	}

	// Close the socket
	if (close(sockedTCP) == -1) {
		logs(L_DEBUG, "PartieManager | partieProcessusManager | close == -1");
		exit(EXIT_FAILURE);
	}

	// Free the memory
	free(th_shared_memory.threads);
	free(th_args_list);

	// Exit the processus
	exit(EXIT_SUCCESS);
}

/**
 * @brief Thread that will handle the TCP connection with a client
 * 
 * @param args	threadTCPArgs structure containing the arguments
 * 
 * @return void* (NULL)
*/
void* partieThreadTCP(void *thread_args) {
	// Get the arguments in a local variable
	threadTCPArgs *args = (threadTCPArgs*)thread_args;

	// Print messages that the thread is running
	logs(L_DEBUG, "PartieManager | partieThreadTCP | Thread %d started", args->threadId);

	// Make the client join the game
	joinPartieTCP(args, args->sharedMemory);

	// Wait for client requests until the client leaves the game
	int8_t loop = 1;
	while (loop) {
		
		// Read the request
		NetMessage request;
		if (recvfrom(args->clientSocket, &request, sizeof(NetMessage), 0, NULL, NULL) == -1) {
			logs(L_DEBUG, "PartieManager | partieThreadTCP | recvfrom == -1");
			exit(EXIT_FAILURE);
		}

		// Handle the request
		switch (request.type) {

			case TCP_REQ_PARTIE_LEAVE:
				leavePartieTCP(args, args->sharedMemory);
				loop = 0;
				break;

			default:
				logs(L_DEBUG, "PartieManager | partieThreadTCP | Unknown request");
		}
	}

	// Close the socket
	if (close(args->clientSocket) == -1) {
		logs(L_DEBUG, "PartieManager | partieThreadTCP | close == -1");
		exit(EXIT_FAILURE);
	}

	// Print messages that the thread is finished
	logs(L_DEBUG, "PartieManager | partieThreadTCP | Thread %d finished", args->threadId);

	// End the thread
	return NULL;
}

/**
 * @brief Make the client join the game by sending informations about the game
 * 
 * @param args			threadTCPArgs structure containing the arguments
 * @param sharedMemory	threadsSharedMemory structure containing the shared memory
 * 
 * @return void
*/
void joinPartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory) {

	// Send informations about the game to the client
	NetMessage response;
	response.type = TCP_REQ_PARTIE_JOIN;
	
	// Fill the response
	// TODO

	// Send the response
	if (sendto(args->clientSocket, &response, sizeof(NetMessage), 0, NULL, 0) == -1) {
		logs(L_DEBUG, "PartieManager | joinPartieTCP | sendto == -1");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Make the client leave the game
 * 
 * @param args			threadTCPArgs structure containing the arguments
 * @param sharedMemory	threadsSharedMemory structure containing the shared memory
 * 
 * @return void
*/
void leavePartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory) {

	// Remove the thread from the list
	sharedMemory->thread_states[args->threadId] = TH_STATE_DISCONNECTED;

	// Delete the client from the game
	// TODO
}

