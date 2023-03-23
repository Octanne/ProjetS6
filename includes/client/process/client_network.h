
#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "net_struct.h"
#include "gui_struct.h"

NetworkSocket init_udp_network(int argc, char *argv[], int *pid_tcp_handler);
int wait_for_tcp_connection(GameInterface *gameI, int tcpPort); // Ouvre son propre processus et retourne le pid
int init_tcp_network(GameInterface *gameI, int tcpPort); // Bloquant le temps de la reception

void close_tcp_network(GameInterface *gameI);
void close_udp_network(GameInterface *gameI);

NetMessage send_udp_message(UDPSocketData *udpSocket, NetMessage *message); // Bloquant le temps de la reception
NetMessage send_tcp_message(TCPSocketData *tcpSocket, NetMessage *message); // Bloquant le temps de la reception

#endif

