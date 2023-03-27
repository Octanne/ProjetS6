
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

// GLOBAL REQUESTS
#define NET_REQ_PING             1 // Fait

// UDP REQUESTS
#define UDP_REQ_PARTIE_LIST      10 // Fait
#define UDP_REQ_MAP_LIST         11 // Fait
#define UDP_REQ_CREATE_PARTIE    12 // Fait
#define UDP_REQ_WAITLIST_PARTIE  13 // Fait

// TCP REQUESTS
#define TCP_REQ_INPUT_PLAYER     22 
#define TCP_REQ_GAME_UPDATE      23 // Fait
#define TCP_REQ_TEXT_INFO_GUI    24 // Fait

#define NET_TIMEOUT   1 // in seconds
#define NET_MAX_TRIES 5

#include "data_update.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct {
    int type;
    union {
        // TCP messages
        DataUpdateGame dataUpdateGame;
        DataInputPlayer dataInputPlayer;
        DataTextInfoGUI dataTextInfoGUI;
        // UDP messages
        PartieListeMessage partieListeMessage;
        MapListeMessage mapListMessage;
        PartieCreateMessage partieCreateMessage;
        PartieJoinLeaveWaitMessage partieWaitListMessage;
    };
} NetMessage;

typedef struct {
    int sockfd;
    struct sockaddr_in serv_addr;

    bool network_init;

    bool waitlist_running;
    NetMessage waitlist_message;
} UDPSocketData;

typedef struct {
    int sockfd;
    struct sockaddr_in serv_addr;

    bool network_init;
    bool read_running;
} TCPSocketData;

typedef struct {
    UDPSocketData udpSocket;
    TCPSocketData tcpSocket;

    pthread_t pid_waitlist;
    pthread_t pid_receive_tcp;
} NetworkSocket;

#endif

