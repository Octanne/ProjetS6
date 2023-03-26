
#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "net_struct.h"
#include "gui_struct.h"

/**
 * @brief Initialise la socket UDP
 * 
 * @param argc 
 * @param argv 
 * @param pid_tcp_handler 
 * @return NetworkSocket 
 */
NetworkSocket *init_udp_network(int argc, char *argv[]); // Non bloquant
/** 
 * @brief Débute la lecture de la socket UDP ^pur le message de connexion TCP, Elle ouvre son propre processus.
 * 
 * @param gameI
 * @param tcpPort
*/
void wait_tcp_connection(GameInterface *gameI, int tcpPort);
/**
 * @brief Stop la lecture de la socket UDP pour le message de connexion TCP
 * 
 * @param gameI 
 */
void stop_wait_tcp_connection(GameInterface *gameI);

/**
 * @brief Stop read tcp socket thread
 * 
 */
void stop_read_tcp_socket(GameInterface *gameI);

void close_tcp_socket(GameInterface *gameI);

/**
 * @brief Initialise la socket TCP et rentre les données dans le GameInterface
 * 
 * @param gameI 
 * @param tcpPort 
 * @return int 
 */
int init_tcp_network(GameInterface *gameI, int tcpPort);

/**
 * @brief Envoie un message UDP
 * 
 * @param udpSocket 
 * @param message 
 * @return NetMessage 
 */
NetMessage send_udp_message(UDPSocketData *udpSocket, NetMessage *message); // Bloquant le temps de la reception
/**
 * @brief Envoie un message TCP
 * 
 * @param tcpSocket 
 * @param message 
 * @return NetMessage 
 */
NetMessage send_tcp_message(TCPSocketData *tcpSocket, NetMessage *message); // Bloquant le temps de la reception

#endif

