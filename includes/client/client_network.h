
#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdbool.h>
#include <netinet/in.h>

#include "net_message.h"

int init_network(int argc, char *argv[]);

bool udp_message_handler(int sockfd, struct sockaddr_in serv_addr);
bool process_udp_message(NetMessage *message, int sockfd, struct sockaddr_in serv_addr); 
bool add_udp_message_to_queue(NetMessage *message);
NetMessage *get_udp_message_from_queue();

#endif

