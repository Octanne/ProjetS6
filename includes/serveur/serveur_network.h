
#ifndef __SERVEUR_NETWORK_H__
#define __SERVEUR_NETWORK_H__

#include <stdbool.h>

int init_network(int argc, char *argv[]);
bool udp_request_handler(int sockfd); // return true if handler should continue
int init_tcp_connection(); // TODO a voir

#endif

