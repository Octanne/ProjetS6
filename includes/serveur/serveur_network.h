
#ifndef __SERVEUR_NETWORK_H__
#define __SERVEUR_NETWORK_H__

#include <stdbool.h>

#include "partie_manager.h"

int init_network(int argc, char *argv[]);
int udp_request_handler(int sockfd, PartieManager *partieManager);
int init_tcp_connection(); 

#endif

