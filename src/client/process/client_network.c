
#include "client_network.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>

#include "utils.h"
#include "constants.h"
#include "net_struct.h"

NetworkSocket init_network(int argc, char *argv[], int *pid_tcp_handler) {
    logs(L_INFO, "Network | Init network...");
    int opt;
    int port = 0;
    char *host = NULL;

    *pid_tcp_handler = 0;

    while ((opt = getopt(argc, argv, "p:h:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-p port] [-h host]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (port == 0 || host == NULL) {
        fprintf(stderr, "Les deux options -p et -h sont obligatoires\n");
        exit(EXIT_FAILURE);
    }

    // Logs the port and the host
    logs(L_INFO, "Network | Port : %d, Host : %s", port, host);
    
    int sockfd;
    struct sockaddr_in serv_addr;

    // Création de la socket avec vérification
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        logs(L_INFO, "Network | Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Initialisation de la structure de l'adresse du serveur
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, host, &serv_addr.sin_addr) != 1) {
        logs(L_INFO, "Network | Error converting address");
        perror("Error converting address");
        exit(EXIT_FAILURE);
    }

    // Configuration du timeout
    struct timeval tv;
    tv.tv_sec = NET_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        logs(L_INFO, "Network | Error setting timeout");
        perror("Error setting timeout");
        exit(EXIT_FAILURE);
    }

    // Create network socket
    NetworkSocket networkSocket;
    networkSocket.pid_tcp_handler = pid_tcp_handler;
    networkSocket.udpSocket.sockfd = sockfd;
    networkSocket.udpSocket.serv_addr = serv_addr;

    // Faire le ping pour vérifier que le serveur est bien là
    NetMessage message;
    message.type = NET_REQ_PING;
    if(process_udp_message(&networkSocket.udpSocket, &message).type != NET_REQ_PING) {
        logs(L_INFO, "Network | Error the server is not responding");
        printf("Network | Error the server is not responding\n");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Network initialized");

    logs(L_INFO, "Network | Return socket data info");

    return networkSocket;
}

NetMessage process_udp_message(UDPSocketData *udpSocket, NetMessage *message) {
    int nb_try = 0;
    NetMessage response;
    bool received = false;
    while (!received && nb_try < NET_MAX_TRIES) {
        // Send message
        if(sendto(udpSocket->sockfd, message, sizeof(NetMessage), 0, (struct sockaddr*)&udpSocket->serv_addr, sizeof(struct sockaddr_in)) == -1) {
            if (errno == EINTR) {
                logs(L_INFO, "Network | Network closed while sending message");
                exit(EXIT_SUCCESS);
            }
            perror("Error sending message");
            logs(L_INFO, "Network | Error sending message");
            exit(EXIT_FAILURE);
        }
        logs(L_INFO, "Network | Message sent");

        // Receive response
        if(recvfrom(udpSocket->sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {
            if (errno == EINTR) {
                logs(L_INFO, "Network | Network closed while receiving response");
                exit(EXIT_SUCCESS);
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                logs(L_INFO, "Network | Timeout");
                if (!udpSocket->network_init) printf("Timeout, try %d/%d...\r", 1+nb_try, NET_MAX_TRIES);
                nb_try++;
            } else {
                perror("Error receiving response");
                logs(L_INFO, "Network | Error receiving response");
                exit(EXIT_FAILURE);
            }
        } else {
            logs(L_INFO, "Network | Message received");

            if (message->type == NET_REQ_PING && response.type == NET_REQ_PING) {
                logs(L_INFO, "Network | Ping received");
                received = true;
            } else {
                logs(L_INFO, "Network | Unknown message received");
                received = false;
            }
            
            // TODO ajouter autre chose que le ping

        }
    }

    return response;
}

