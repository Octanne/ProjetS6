
#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "net_struct.h"
#include "gui_struct.h"

NetworkSocket init_udp_network(int argc, char *argv[]); // Non bloquant
int init_tcp_network(GameInterface *gameI, int tcpPort);

void wait_tcp_connection(GameInterface *gameI, int tcpPort);
void stop_wait_tcp_connection(GameInterface *gameI);
void stop_read_tcp_socket(GameInterface *gameI);
void close_tcp_socket(GameInterface *gameI);

NetMessage send_udp_message(UDPSocketData *udpSocket, NetMessage *message); // Bloquant le temps de la reception
void send_tcp_message(TCPSocketData *tcpSocket, NetMessage *message);

#endif

