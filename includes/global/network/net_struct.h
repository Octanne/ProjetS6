
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#define NET_REQ_PING           1
#define NET_REQ_PARTIE_LIST    2
#define NET_REQ_MAP_LIST       3
#define NET_REQ_PARTIE_JOIN    4
#define NET_REQ_PARTIE_LEAVE   5
#define NET_REQ_PARTIE_CREATE  6

#define NET_TIMEOUT 2 // in seconds
#define NET_MAX_TRIES 3

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
        DataTextInfoGUI textInfoGUI;
        DataUpdateGame updateGame;
        DataInputPlayer inputPlayer;

        PartieListMessage partieListMessage;
        MapListeMessage mapListMessage;
    };
} NetMessage;

#endif

