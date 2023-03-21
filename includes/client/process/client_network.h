
#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "net_struct.h"

NetworkSocket init_network(int argc, char *argv[], int *pid_tcp_handler);
NetMessage process_udp_message(UDPSocketData *udpSocket, NetMessage *message); // Bloquant le temps de la reception

#endif

