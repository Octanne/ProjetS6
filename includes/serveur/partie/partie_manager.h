
#ifndef __PARTIE_MANAGER_H__
#define __PARTIE_MANAGER_H__

#include <stdbool.h>
#include <netinet/in.h>
#include <pthread.h>

#include "liste.h"
#include "data_update.h"
#include "net_struct.h"
#include "player.h"
#include "level.h"

typedef struct {
	char map[255];
	
	int isStart;
	int nbPlayers;
	int maxPlayers;

	int portTCP;
	bool tcpStart;
	int pid_partie_process;

	Liste playersInWait;
} PartieStatutInfo;

typedef struct {
	Liste partieInfoListe;
	UDPSocketData udpSocket;
} PartieManager;

PartieManager partieManager_create(UDPSocketData udpSocket);
void initTCPServersPIDList();
void killTCPServersPID();

void updatePartieListe(PartieManager *partieManager);

// ### UDP ###
PartieListeMessage listPartie(PartieManager *partieManager, int numPage);
MapListeMessage listMaps(PartieManager *partieManager, int numPage);
PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap, struct sockaddr_in clientAddr);
PartieJoinLeaveWaitMessage waitListePartie(PartieManager *partieManager, int numPartie, bool waitState, struct sockaddr_in clientAddr);

// ### TCP ###
#define TH_STATE_DISCONNECTED -1
#define TH_STATE_CONNECTED 1

typedef struct {
    LevelMutex *levelMutex;
    Objet *door;
} Door;

typedef struct {
    Door door1;
    Door door2;
	bool isLinked;
} DoorLink;

typedef struct {
    LevelMutex *levelMutex;
	Objet *mob;
	bool isFreeze;

	pthread_t thread;
} MobThreadsArgs;

typedef struct {
    LevelMutex *levelMutex;
	Objet *piege;
} PiegeLoaded;

typedef struct {
	pthread_mutex_t mutex;				// Pthread mutex		Used to lock the shared memory
	pthread_t *threads;					// List of threads
	int *thread_states;					// Thread states (disconnected, connected, etc.)
	int *thread_sockets;				// Thread sockets
	int nbThreads;						// Number of threads

	// Game data
	int game_state;						// Game state (0 = waiting, 1 = playing, 2 = end)
	pthread_cond_t update_cond;			// Pthread condition	Used to wait for an update
	Player *players;					// Store players to send efficiently to clients
	Liste levels;						// Store levels and mutex in a list
	DoorLink *doors;					// Store doors in array
	Liste mobsThreadsArgs;				// Store mobs in a list (probe and robot)
	Liste piegesLoaded;					// Store pieges in a list
	pthread_t piegeThread; 		    	// Store pieges thread
	short enterX;						// Enter X
	short enterY;						// Enter Y
	short levelEnter;					// Enter level
} threadsSharedMemory;

typedef struct {
	int threadId;
	int clientSocket;
} threadTCPArgs;

int startPartieProcessus(PartieManager *partieManager, PartieStatutInfo *partieInfo);
void partieProcessusManager(int sockedTCP, PartieStatutInfo partieInfo);
void* partieThreadTCP(void *thread_args);

void updatePartieTCP(threadsSharedMemory *sharedMemory);
void leavePartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory);
void inputPartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory, int input);

void broadcastMessage(threadsSharedMemory *sharedMemory, char* message , int color, int line);
void privateMessage(threadsSharedMemory *sharedMemory, int clientThreadId, char* message , int color, int line);

#endif

