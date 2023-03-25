
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

// GLOBAL REQUESTS
#define NET_REQ_PING             1 // Fait

// UDP REQUESTS
#define UDP_REQ_PARTIE_LIST      10 // Fait
#define UDP_REQ_MAP_LIST         11 // Fait
#define UDP_REQ_CREATE_PARTIE    12 // Fait
#define UDP_REQ_WAITLIST_PARTIE  13

// TCP REQUESTS
#define TCP_REQ_PARTIE_JOIN      20
#define TCP_REQ_PARTIE_LEAVE     21

#define NET_TIMEOUT   1 // in seconds
#define NET_MAX_TRIES 5

#include "data_update.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

typedef struct {
    int sockfd;
    struct sockaddr_in serv_addr;

    bool network_init;
} UDPSocketData;

typedef struct {
    int sockfd;
    // TODO ajouter les autres trucs
} TCPSocketData;

typedef struct {
    UDPSocketData udpSocket;
    TCPSocketData tcpSocket;

    int **pid_tcp_handler;
} NetworkSocket;

typedef struct {
    int type;
    union {
        // TCP messages
        DataUpdateGame dataUpdateGame;
        DataInputPlayer dataInputPlayer;
        DataTextInfoGUI dataTextInfoGUI;
        // TODO ADD JOIN AND LEAVE
        // UDP messages
        PartieListeMessage partieListeMessage;
        MapListeMessage mapListMessage;
        PartieCreateMessage partieCreateMessage;
        PartieJoinLeaveWaitMessage partieWaitListMessage;
    };
} NetMessage;

#endif

