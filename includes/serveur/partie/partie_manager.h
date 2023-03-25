
#ifndef __PARTIE_MANAGER_H__
#define __PARTIE_MANAGER_H__

#include <stdbool.h>
#include <netinet/in.h>
#include <pthread.h>

#include "liste.h"
#include "data_update.h"
#include "net_struct.h"

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
	Liste players;
	Liste load_level;

	PartieStatutInfo *infosStatus;
} Partie;

typedef struct {
	Liste partieInfoListe;
	UDPSocketData udpSocket;
} PartieManager;

PartieManager partieManager_create(UDPSocketData udpSocket);


// ### UDP ###
PartieListeMessage listPartie(PartieManager *partieManager, int numPage);
MapListeMessage listMaps(PartieManager *partieManager, int numPage);
PartieCreateMessage createPartie(PartieManager *partieManager, int maxPlayers, int numMap, struct sockaddr_in clientAddr);
PartieJoinLeaveWaitMessage waitListePartie(PartieManager *partieManager, int numPartie, bool waitState, struct sockaddr_in clientAddr);

// ### TCP ###
#define TH_STATE_DISCONNECTED -1

typedef struct {
	pthread_mutex_t mutex;	// Pthread mutex		USELESS FOR NOW
	pthread_cond_t cond;	// Pthread condition	USELESS FOR NOW
	pthread_t *threads;		// List of threads
	int *thread_states;		// Thread states (disconnected, connected, etc.)
	int nbThreads;			// Number of threads
} threadsSharedMemory;

typedef struct {
	int threadId;
	int clientSocket;
	threadsSharedMemory *sharedMemory;
} threadTCPArgs;

int startPartieProcessus(PartieManager *partieManager, PartieStatutInfo *partieInfo);
void partieProcessusManager(int sockedTCP, PartieStatutInfo partieInfo);
void* partieThreadTCP(void *thread_args);

void joinPartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory);
void leavePartieTCP(threadTCPArgs *args, threadsSharedMemory *sharedMemory);

#endif

