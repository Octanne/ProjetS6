
#include "partie_manager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include "utils.h"
#include "constants.h"
#include "system_save.h"
#include "level_update.h"
#include "level_action.h"

#define NUM_PARTIES_PAR_PAGE 4

threadsSharedMemory th_shared_memory;

/**
 * @brief Function handling the SIGINT signal for TCP servers
*/
void sigintHandler(int sig_num) {
	// Logs and print
	logs(L_INFO, "PartieManager | TCP Server | SIGINT signal received");
	printf("PartieManager | TCP Server | SIGINT signal received\n");

	// Mutex lock
	pthread_mutex_lock(&th_shared_memory.mutex);

	// Send to all players a leave message
	NetMessage message;
	message.type = TCP_REQ_PARTIE_LEAVE;
	int i;
	for (i = 0; i < th_shared_memory.nbThreads; i++) {
		if (th_shared_memory.thread_states[i] != TH_STATE_DISCONNECTED) {

			// Send the leave message
			if (write(th_shared_memory.thread_sockets[i], &message, sizeof(NetMessage)) == -1) {
				perror("PartieManager | TCP Server | Send leave message");
			}

			// Logs and print
			logs(L_INFO, "PartieManager | TCP Server | Send leave message to player %d", i);
			printf("PartieManager | TCP Server | Send leave message to player %d\n", i);

			// Close the socket
			if (close(th_shared_memory.thread_sockets[i]) == -1) {
				perror("PartieManager | TCP Server | Close socket");
			}

			// Set the thread state to disconnected
			th_shared_memory.thread_states[i] = TH_STATE_DISCONNECTED;
		}
	}

	// Unlock mutex
	pthread_mutex_unlock(&th_shared_memory.mutex);

	// Exit the process
	logs(L_INFO, "PartieManager | TCP Server | Exit process");
	printf("PartieManager | TCP Server | Exit process\n");
	exit(EXIT_SUCCESS);
}

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

Liste tcpServersPID;
/**
 * @brief Initialize the list of TCP servers PID to kill them when the server is closed
*/
void initTCPServersPIDList() {
	tcpServersPID = liste_create(true);
	logs(L_INFO, "PartieManager | Init TCP Servers PID List");
	printf("PartieManager | Init TCP Servers PID List\n");
}

/**
 * @brief Send a Kill signal to all the TCP servers PID
*/
void killTCPServersPID() {
	logs(L_INFO, "PartieManager | Kill TCP Servers PID List");
	printf("PartieManager | Kill TCP Servers PID List\n");

	// Loop through the list of TCP servers PID
	EltListe *elt = tcpServersPID.tete;
	while (elt != NULL) {
		int pid = *(int*)elt->elmt;
		kill(pid, SIGINT);
		elt = elt->suivant;

		// Wait for the child process to close
		waitpid(pid, NULL, 0);

		// Logs and print
		logs(L_INFO, "PartieManager | TCP Server PID (%d) killed", pid);
		printf("PartieManager | TCP Server PID (%d) killed\n", pid);
	}

	// Logs and print
	logs(L_INFO, "PartieManager | TCP Servers PID List killed");
	printf("PartieManager | TCP Servers PID List killed\n");

	// Free the list
	liste_free(&tcpServersPID, true);
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
		int numPartie = i + (numPage - 1) * NUM_PARTIES_PAR_PAGE;
		PartieStatutInfo *partieInfo = (PartieStatutInfo*)liste_get(&partieManager->partieInfoListe, numPartie);

		// If the game exists, add it to the list
		if (partieInfo != NULL) {
			partieListeMessage.partieInfo[i].numPartie = numPartie;
			partieListeMessage.partieInfo[i].set = true;
			partieListeMessage.partieInfo[i].maxPlayers = partieInfo->maxPlayers;
			partieListeMessage.partieInfo[i].nbPlayers = partieInfo->nbPlayers;
			partieListeMessage.partieInfo[i].status = partieInfo->isStart;
			strcpy(partieListeMessage.partieInfo[i].name, partieInfo->map);

			// Print the game
			printf("PartieManager | Map = %s, numPartie = %d, MaxPlayers = %d, Players = %d\n", partieListeMessage.partieInfo[i].name, 
				partieListeMessage.partieInfo[i].numPartie, partieListeMessage.partieInfo[i].maxPlayers, partieListeMessage.partieInfo[i].nbPlayers);
			partieListeMessage.nbParties++;
		}
		else
			partieListeMessage.partieInfo[i].set = false;
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
 * @brief Create a list of maps on the game depending on the page number to send to client
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
		int numMap = i + (numPage - 1) * 4;
		MapInfo *mapInfo = (MapInfo*)liste_get(&mapListe, numMap);

		// If the map exists, add it to the list
		if (mapInfo != NULL) {
			mapListeMessage.mapInfo[i].numMap = numMap;
			mapListeMessage.mapInfo[i].set = true;
			strcpy(mapListeMessage.mapInfo[i].name, mapInfo->name);
			mapListeMessage.nbMaps++;
		} else
			mapListeMessage.mapInfo[i].set = false;
	}

	// Free the map list and return the list of maps
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
 * @return PartieCreateMessage to send to the client
*/
PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap, struct sockaddr_in clientAddr) {

	// Initialize the structure
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
			partieJoinLeaveWaitMessage.takeInAccount = true;
			logs(L_INFO, "PartieManager | TCPServer started on port %d", partieInfo->portTCP);
			printf("PartieManager | TCPServer started on port %d\n", partieInfo->portTCP);

			// Send the port to the clients in the wait list
			NetMessage netMessage;
			netMessage.type = UDP_REQ_WAITLIST_PARTIE;
			netMessage.partieWaitListMessage = partieJoinLeaveWaitMessage;
			EltListe *elt = partieInfo->playersInWait.tete;
			while (elt != NULL) {
				
				// Get the client address
				struct sockaddr_in *clientAddrInListe = (struct sockaddr_in*)elt->elmt;

				// Logs and print
				logs(L_INFO, "PartieManager | Sending TCP Port to the client : %s:%d", inet_ntoa(clientAddrInListe->sin_addr), ntohs(clientAddrInListe->sin_port));
				printf("PartieManager | Sending TCP Port to the client : %s:%d\n", inet_ntoa(clientAddrInListe->sin_addr), ntohs(clientAddrInListe->sin_port));

				// Send the message to the client
				if (sendto(partieManager->udpSocket.sockfd, &netMessage, sizeof(NetMessage), 0, (struct sockaddr*)clientAddrInListe, sizeof(struct sockaddr_in)) == -1) {
					logs(L_DEBUG, "PartieManager | Error while sending the port to the client");
					printf("PartieManager | Error while sending the port to the client\n");
				}
				// Go to the next client
				elt = elt->suivant;
			}

			// Add last client to the wait list (the client who send the request)
			struct sockaddr_in *clientAddrCopy = malloc(sizeof(struct sockaddr_in));
			memcpy(clientAddrCopy, &clientAddr, sizeof(struct sockaddr_in));
			liste_add(&partieInfo->playersInWait, clientAddrCopy, TYPE_SOCKADDR_IN);
			logs(L_INFO, "PartieManager | Last client added to the wait list");
			printf("PartieManager | Last client added to the wait list\n");
		}
	}
	else {
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
 * @brief Start the partie processus and register it
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

	// Listen on the socket
	logs(L_DEBUG, "PartieManager | startPartieProcessus | Listen on the socket");
	if (listen(socketTCP, 10) == -1) {
		logs(L_DEBUG, "PartieManager | startPartieProcessus | listen == -1");
		exit(EXIT_FAILURE);
	}

	// Fill the PartieStatutInfo
	partieInfo->portTCP = ntohs(socketAddrOut.sin_port);

	// Create the processus that will handle the socket
	pid_t pid = fork();
	if (pid == 0) {
		// Child processus (partie processus)
		// Register SIGINT handler
		struct sigaction sa;
		sa.sa_handler = sigintHandler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		if (sigaction(SIGINT, &sa, NULL) == -1) {
			logs(L_DEBUG, "PartieManager | startPartieProcessus | sigaction == -1");
			printf("PartieManager | startPartieProcessus | Error while registering the SIGINT handler");
			return -1;
		}

		// Close the UDP socket
		if (close(partieManager->udpSocket.sockfd) == -1) {
			logs(L_DEBUG, "PartieManager | startPartieProcessus | close == -1");
			printf("PartieManager | startPartieProcessus | Error while closing the UDP socket");
			return -1;
		}

		// Launch the TCPServer manager with a copy of PartieStatutInfo (copy because this is a child process)
		partieProcessusManager(socketTCP, *partieInfo);
	}
	
	// Parent processus (PartieManager processus)
	// Close the TCP socket (the child processus will handle it)
	if (close(socketTCP) == -1) {
		logs(L_DEBUG, "PartieManager | startPartieProcessus | close == -1");
		printf("PartieManager | startPartieProcessus | Error while closing the TCP socket");
		return -1;
	}

	// Register the processus in the tcpServersPID list
	pid_t *pidCopy = malloc(sizeof(pid_t));
	*pidCopy = pid;
	liste_add(&tcpServersPID, pidCopy, TYPE_PID);

	return 0;
}

/**
 * @brief Partie processus manager
 * - Wait for clients to connect to the TCPServer
 * - Launch a thread for each client
 * 
 * @param sockedTCP		Socket TCP file descriptor (binded)
 * @param partieInfo	PartieStatutInfo containing informations about the game
*/
void partieProcessusManager(int sockedTCP, PartieStatutInfo partieInfo) {
	int i;

	// Create threadsSharedMemory and create thread list depending on the number of players
	memset(&th_shared_memory, 0, sizeof(threadsSharedMemory));
	th_shared_memory.threads = malloc(sizeof(pthread_t) * partieInfo.nbPlayers);
	th_shared_memory.thread_states = malloc(sizeof(int) * partieInfo.nbPlayers);
	th_shared_memory.thread_sockets = malloc(sizeof(int) * partieInfo.nbPlayers);
	th_shared_memory.nbThreads = partieInfo.nbPlayers;
	memset(th_shared_memory.thread_states, 0, sizeof(int) * partieInfo.nbPlayers);

	// Define game part
	th_shared_memory.game_state = 0;
	pthread_cond_init(&th_shared_memory.update_cond, NULL);

	// Alocate temporary liste of doors
	Liste doorListe = liste_create(true);

	// Allocate liste of mobs threads arguments
	th_shared_memory.mobsThreadsArgs = liste_create(true);

	// Load all the levels in the server
	th_shared_memory.levels = liste_create(true);
	char lvlPath[260];
	sprintf(lvlPath, "maps/%s", partieInfo.map);
	file_t level_file = load_file(lvlPath);
	int loop = 0;
	i = 0;
	while (loop == 0) {

		// Try to load the level
		Level *level = malloc(sizeof(Level));
		loop = get_level(level_file, i++, level);
		level->levelNumber = i - 1;

		// Add the level to the list if it was loaded, else free the memory
		if (loop == 0) {
			liste_add(&th_shared_memory.levels, level, TYPE_LEVEL);
			
			// Lecture des objets du niveau
			EltListe *elt = level->listeObjet.tete;
			while (elt != NULL) {
				Objet *o = elt->elmt;
				// Add the door to the list of doors
				if (o->type == DOOR_ID) {
					// Ajout à la liste des portes
					Door *door = malloc(sizeof(Door));
					door->door = o;
					door->level = level;
					liste_add(&doorListe, door, TYPE_DOOR);
				} else if (o->type == ROBOT_ID || o->type == PROBE_ID) {
					// Add the mob to the list of mobs
					MobThreadsArgs *args = malloc(sizeof(MobThreadsArgs));
					args->mob = o;
					args->level = level;
					liste_add(&th_shared_memory.mobsThreadsArgs, args, TYPE_MOBTHREAD_ARGS);
				}

				// Next element
				elt = elt->suivant;
			}

			for (int j = 0; j < level->listeObjet.taille; j++) {
				Objet *o = liste_get(&level->listeObjet, j);
				// Add the door to the list of doors
				if (o != NULL && o->type == DOOR_ID) {
					// Ajout à la liste des portes
					Door *door = malloc(sizeof(Door));
					door->door = o;
					door->level = level;
					liste_add(&doorListe, door, TYPE_DOOR);
				}
			}
		}
		else
			free(level);
	}
	
	// Links the doors
	th_shared_memory.doors = create_doorlink(&doorListe); // Dynamic array of DoorLink
	liste_free(&doorListe, false);

	// TODO : Start threads for mobs (PROBE AND ROBOT) (one thread per mob) (see mobsThreadsArgs)

	// Search start block of the first level
	short enterX = -1, enterY = -1;
	EltListe *e = th_shared_memory.levels.tete;
	Level* level = (Level*)e->elmt;
	while (e != NULL && enterX == -1 && enterY == -1) {
		level = (Level*)e->elmt;

		EltListe *eltO = level->listeObjet.tete;
		while (eltO != NULL) {
			// Get the object
			Objet *o = (Objet*)eltO->elmt;
			if (o->type == START_ID) {
				// Save the position of the start block
				logs(L_DEBUG, "PartieManager | partieProcessusManager | Start block found at (%d, %d)", o->x, o->y);
				printf("PartieManager | partieProcessusManager | Start block found at (%d, %d)\n", o->x, o->y);
				enterX = o->x;
				enterY = o->y;
				break;
			}

			// Next object
			eltO = eltO->suivant;
		}

		// Next level
		e = e->suivant;
	}

	// Place the players on the map
	th_shared_memory.players = malloc(sizeof(Player) * partieInfo.nbPlayers);
	for (i = 0; i < partieInfo.nbPlayers; i++) {
		Player p;
		char name[255];
		sprintf(name, "Player %d", i);
		strcpy(p.name, name);
		p.life = 5;
		p.nbBombs = 0;
		p.isAlive = true;
		p.isFreeze = false;
		p.isInvincible = false;
		p.key1 = false;
		p.key2 = false;
		p.key3 = false;
		p.key4 = false;
		p.posX = enterX;
		p.posY = enterY;
		p.level = 1;
		p.obj = poserPlayer(level, enterX, enterY);
		if (p.obj == NULL) {
			logs(L_DEBUG, "PartieManager | partieProcessusManager | poserPlayer == NULL");
			printf("PartieManager | partieProcessusManager | Error while placing the player on the map\n");
			exit(EXIT_FAILURE);
		}
		p.obj->player.color = 1 + i % 7;
		p.obj->player.orientation = RIGHT_ORIENTATION;

		// Save the player in the shared memory
		th_shared_memory.players[i] = p;
	}

	// Create threadTCPArgs list to pass arguments to threads
	threadTCPArgs *th_args_list = malloc(sizeof(threadTCPArgs) * partieInfo.nbPlayers);

	// Wait for clients to connect
	logs(L_DEBUG, "PartieManager | partieProcessusManager | Waiting for %d clients to connect", partieInfo.nbPlayers);
	printf("PartieManager | partieProcessusManager | Waiting for %d clients to connect\n", partieInfo.nbPlayers);
	for (i = 0; i < partieInfo.nbPlayers; i++) {

		// Accept a client
		struct sockaddr_in clientAddr;
		socklen_t clientAddrSize = sizeof(clientAddr);
		int clientSocket = accept(sockedTCP, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (clientSocket == -1) {
			logs(L_DEBUG, "PartieManager | partieProcessusManager | accept == -1");
			exit(EXIT_FAILURE);
		}

		// Logs
		logs(L_DEBUG, "PartieManager | partieProcessusManager | Client %d connected with clientSocket %d", i, clientSocket);
		printf("PartieManager | partieProcessusManager | Client %d connected with clientSocket %d\n", i, clientSocket);

		// Link arguments to the thread
		th_args_list[i].threadId = i;
		th_args_list[i].clientSocket = th_shared_memory.thread_sockets[i] = clientSocket;
		logs(L_DEBUG, "PartieManager | partieProcessusManager | Thread %d linked to clientSocket %d", i, clientSocket);
		printf("PartieManager | partieProcessusManager | Thread %d linked to clientSocket %d\n", i, clientSocket);

		// Create the thread
		int status = pthread_create(&th_shared_memory.threads[i], NULL, partieThreadTCP, &th_args_list[i]);
		if (status != 0) {
			logs(L_DEBUG, "PartieManager | partieProcessusManager | pthread_create != 0");
			exit(EXIT_FAILURE);
		}

		// Logs
		logs(L_DEBUG, "PartieManager | partieProcessusManager | Thread %d created", i);
		printf("PartieManager | partieProcessusManager | Thread %d created\n", i);
	}


	// Lock mutex and set game_state to 1
	pthread_mutex_lock(&th_shared_memory.mutex);
	th_shared_memory.game_state = 1;

	// While the game is not finished,
	while (th_shared_memory.game_state == 1) {

		// Send an update to all clients and wait for an update
		updatePartieTCP(&th_shared_memory);
		pthread_cond_wait(&th_shared_memory.update_cond, &th_shared_memory.mutex);

		// Check if there is remaining players
		int remainingPlayers = 0;
		for (i = 0; i < partieInfo.nbPlayers; i++)
			if (th_shared_memory.thread_states[i] != TH_STATE_DISCONNECTED)
				remainingPlayers++;
		if (remainingPlayers == 0)
			th_shared_memory.game_state = 2;
	}

	// Unlock mutex
	pthread_mutex_unlock(&th_shared_memory.mutex);

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
	liste_free(&th_shared_memory.mobsThreadsArgs, true);
	liste_free(&th_shared_memory.levels, true);
	free(th_shared_memory.threads);
	free(th_args_list);
	free(th_shared_memory.thread_states);
	free(th_shared_memory.players);
	free(th_shared_memory.thread_sockets);
	free(th_shared_memory.doors);

	// Exit the processus with success
	logs(L_DEBUG, "PartieManager | partieProcessusManager | Processus %d ended", getpid());
	printf("PartieManager | partieProcessusManager | Processus %d ended\n", getpid());
	exit(EXIT_SUCCESS);
}

/**
 * @brief Thread that will handle the TCP connection with a client
 * 
 * @param args	threadTCPArgs structure containing the arguments* (NULL)
*/
void* partieThreadTCP(void *thread_args) {
	// Get the arguments in a local variable
	threadTCPArgs *args = (threadTCPArgs*)thread_args;

	// Print messages that the thread is running
	logs(L_DEBUG, "PartieManager | partieThreadTCP | Thread %d started", args->threadId);

	// Wait for client requests until the client leaves the game
	int8_t loop = 1;
	while (loop) {
		
		// Read the request
		NetMessage request;
		int readBytes = read(args->clientSocket, &request, sizeof(NetMessage));
		if (readBytes == -1) {
			logs(L_DEBUG, "PartieManager | partieThreadTCP | read == -1, ernno = %s", strerror(errno));
			printf("PartieManager | partieThreadTCP | Error while reading the request from client %d, errno = %s\n", args->threadId, strerror(errno));
			request.type = TCP_REQ_PARTIE_LEAVE;
		}
		else if (readBytes == 0) {
			logs(L_DEBUG, "PartieManager | partieThreadTCP | Client %d has closed the connection", args->threadId);
			printf("PartieManager | partieThreadTCP | Client %d has closed the connection\n", args->threadId);
			request.type = TCP_REQ_PARTIE_LEAVE;
		}

		// Handle the request
		switch (request.type) {

			case TCP_REQ_PARTIE_LEAVE:
				leavePartieTCP(args, &th_shared_memory);
				loop = 0;
				break;
			
			case TCP_REQ_INPUT_PLAYER:
				inputPartieTCP(args, &th_shared_memory, request.dataInputPlayer.keyPress);
				break;

			default:
				logs(L_DEBUG, "PartieManager | partieThreadTCP | Unknown request : %d", request.type);
				printf("PartieManager | partieThreadTCP | Unknown request : %d\n", request.type);
		}
	}

	// Close the socket
	if (close(args->clientSocket) == -1) {
		logs(L_DEBUG, "PartieManager | partieThreadTCP | close == -1");
		exit(EXIT_FAILURE);
	}

	// Print messages that the thread is finished
	logs(L_DEBUG, "PartieManager | partieThreadTCP | Thread %d finished", args->threadId);
	printf("PartieManager | partieThreadTCP | Thread %d finished\n", args->threadId);

	// End the thread
	return NULL;
}

/**
 * @brief Sending informations about the game to the client to update their game
 * mutex is locked before calling this function, no worries
 * 
 * @param args			threadTCPArgs structure containing the arguments
 * @param sharedMemory	threadsSharedMemory structure containing the shared memory
*/
void updatePartieTCP(threadsSharedMemory *sharedMemory) {

	// Prepare the response
	NetMessage response;
	response.type = TCP_REQ_GAME_UPDATE;
	logs(L_DEBUG, "PartieManager | updatePartieTCP | Preparing response");
	printf("PartieManager | updatePartieTCP | Preparing response\n");
	
	// Send to all connected players the informations about the game
	int i, j;
	EltListe* elt = NULL;
	for (i = 0; i < sharedMemory->nbThreads; i++) {
		if (sharedMemory->thread_states[i] != TH_STATE_DISCONNECTED) {

			// Get head of the level list
			elt = sharedMemory->levels.tete;
			logs(L_DEBUG, "PartieManager | updatePartieTCP | Finding player's level");
			printf("PartieManager | updatePartieTCP | Finding player's level\n");
			
			// Depending on the room of the player, send the informations about the game
			for (j = 1; j <= sharedMemory->levels.taille; j++) {

				// If the player is in the room
				if (sharedMemory->players[i].level == j) {
					logs(L_DEBUG, "PartieManager | updatePartieTCP | Sending update to client %d", i);
					printf("PartieManager | updatePartieTCP | Sending update to client %d\n", i);

					// Get the level
					Level* lvl = (Level*)elt->elmt;
					
					// Fill the response
					response.dataUpdateGame.player = sharedMemory->players[i];
					char* levelBytes = convert_level_to_bytes(*lvl, &response.dataUpdateGame.sizeLevel);
					// copy levelBytes to response.dataUpdateGame.level (char[response.dataUpdateGame.sizeLevel])
					memcpy(&response.dataUpdateGame.levelBytes, levelBytes, response.dataUpdateGame.sizeLevel);
					free(levelBytes);

					// Send the response
					if (write(sharedMemory->thread_sockets[i], &response, sizeof(NetMessage)) == -1) {
						logs(L_DEBUG, "PartieManager | updatePartieTCP | send == -1");
						printf("PartieManager | updatePartieTCP | Error while sending the response to the client %d\n", i);
					}

					// Logs
					logs(L_DEBUG, "PartieManager | updatePartieTCP | Sent update to client %d, size : %ld", i, response.dataUpdateGame.sizeLevel);
					printf("PartieManager | updatePartieTCP | Sent update to client %d, size : %ld\n", i, response.dataUpdateGame.sizeLevel);
				}

				// Go to the next room
				elt = elt->suivant;
			}
		}
	}
}

/**
 * @brief Make the client leave the game
 * 
 * @param args			threadTCPArgs structure containing the arguments
 * @param sharedMemory	threadsSharedMemory structure containing the shared memory
*/
void leavePartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory) {

	// Lock the mutex
	pthread_mutex_lock(&sharedMemory->mutex);

	// Remove the thread from the list
	sharedMemory->thread_states[args->threadId] = TH_STATE_DISCONNECTED;

	// Delete the client from the game
	sharedMemory->players[args->threadId].isAlive = false;

	// Remove the player from the level
	Objet* player = sharedMemory->players[args->threadId].obj;
	Level* lvl = liste_get(&sharedMemory->levels, sharedMemory->players[args->threadId].level);
	levelSupprimerObjet(lvl, player);

	// Signal condition variable
	pthread_cond_broadcast(&sharedMemory->update_cond);

	// Unlock the mutex
	pthread_mutex_unlock(&sharedMemory->mutex);
}

/**
 * @brief Handle the input of the client
 * 
 * @param args			threadTCPArgs structure containing the arguments
 * @param sharedMemory	threadsSharedMemory structure containing the shared memory
 * @param input			Input of the client (key pressed)
*/
void inputPartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory, int input) {

	// For debug
	logs(L_DEBUG, "PartieManager | inputPartieTCP | Thread %d input %d", args->threadId, input);
	printf("PartieManager | inputPartieTCP | Thread %d input %d\n", args->threadId, input);

	// Lock the mutex
	pthread_mutex_lock(&sharedMemory->mutex);

	// Handle the input
	Player* player = &sharedMemory->players[args->threadId];
	Level *lvl = liste_get(&sharedMemory->levels, player->level-1);
	short newX = player->obj->x;
	short newY = player->obj->y;
	
	// Function to handle the player possible movement	
	switch (input) {
		case KEY_UP:
			newY--;
			player_action(player, lvl, newX, newY, sharedMemory);
			break;
		case KEY_DOWN:
			newY++;
			player_action(player, lvl, newX, newY, sharedMemory);
			break;
		case KEY_LEFT:
			newX--;
			player_action(player, lvl, newX, newY, sharedMemory);
			break;
		case KEY_RIGHT:
			newX++;
			player_action(player, lvl, newX, newY, sharedMemory);
			break;
		case KEY_SPACE:
			// On vérifie si le joueur a des bombes
			if (player->nbBombs > 0) {
				// On décrémente le nombre de bombes du joueur
				player->nbBombs--;
				// On créée la bombe
				Objet *bombe = creerBombeExplosif(player->obj->x+1, player->obj->y);
				// On ajoute la bombe au niveau
				levelAjouterObjet(lvl, bombe);
				// On démarre le threads de la bombe
				launch_bomb_routine(sharedMemory, bombe, lvl);
				privateMessage(args, sharedMemory, "Bombe posée !", YELLOW_COLOR, 1);
			} else {
				privateMessage(args, sharedMemory, "Vous n'avez plus de bombes !", RED_COLOR, 1);
			}
			break;
		case KEY_VALIDATE: 
			// On récupère la liste des objets dans la hitbox du joueur
			Liste objCollide = objectInHitBox(lvl, player->obj->x, player->obj->y, 3, 4);
			
			// On parcourt la liste
			EltListe *elt = objCollide.tete;
			while (elt != NULL) {
				Objet *obj = (Objet*)elt->elmt;
				// Si l'objet est une porte
				if (obj->type == DOOR_ID) {
					// On récupère la doorLink
					DoorLink doorLink = sharedMemory->doors[obj->door.numdoor];
					// Si joueur à la door 1
					if (doorLink.door1.door == obj) {
						// On déplace le joueur à la door 2
						changePlayerOfLevel(sharedMemory, player, lvl, doorLink.door2.level, 
							doorLink.door2.door->x, doorLink.door2.door->y);
					} else {
						// On déplace le joueur à la door 1
						changePlayerOfLevel(sharedMemory, player, lvl, doorLink.door1.level, 
							doorLink.door1.door->x, doorLink.door1.door->y);
					}
					break;
				}

				// On passe à l'élément suivant
				elt = elt->suivant;
			}
			break;
		default:
			// SEND la touche au client
			char message[255];
			sprintf(message, "Touche '%d' non prise en compte", input);
			privateMessage(args, sharedMemory, message, RED_COLOR, 1);
			break;
	}

	// Signal condition variable
	pthread_cond_broadcast(&sharedMemory->update_cond);
	pthread_mutex_unlock(&sharedMemory->mutex);
}

/**
 * @brief Envoie un message à tous les clients de la partie
 * 
 * @param sharedMemory structure containing the shared memory
 */
void broadcastMessage(threadsSharedMemory *sharedMemory, char* message , int color, int line) {
	NetMessage response;
	response.type = TCP_REQ_TEXT_INFO_GUI;
	sprintf(response.dataTextInfoGUI.text, message);
	response.dataTextInfoGUI.color = WHITE_COLOR;
	response.dataTextInfoGUI.line = line;

	// Send the response
	for (int i = 0; i < sharedMemory->nbThreads; i++) {
		if (sharedMemory->thread_states[i] != TH_STATE_DISCONNECTED) {
			if (write(sharedMemory->thread_sockets[i], &response, sizeof(NetMessage)) == -1) {
				logs(L_DEBUG, "PartieManager | inputPartieTCP | sendto == -1");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/**
 * @brief Envoie un message à un client de la partie
 * 
 * @param args 
 * @param sharedMemory 
 * @param message 
 * @param color 
 * @param line 
 */
void privateMessage(threadTCPArgs *args, threadsSharedMemory *sharedMemory, char* message , int color, int line) {
	NetMessage response;
	response.type = TCP_REQ_TEXT_INFO_GUI;
	sprintf(response.dataTextInfoGUI.text, message);
	response.dataTextInfoGUI.color = WHITE_COLOR;
	response.dataTextInfoGUI.line = line;

	// Send the response
	if (write(sharedMemory->thread_sockets[args->threadId], &response, sizeof(NetMessage)) == -1) {
		logs(L_DEBUG, "PartieManager | inputPartieTCP | sendto == -1");
		exit(EXIT_FAILURE);
	}
}

