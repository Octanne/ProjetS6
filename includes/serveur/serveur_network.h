
#ifndef __SERVEUR_NETWORK_H__
#define __SERVEUR_NETWORK_H__

int init_network(int argc, char *argv[]);

int udp_request_handler(int sockfd);

int init_tcp_connection(); // TODO a voir

#endif

