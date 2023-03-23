
#include "client_network.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "utils.h"
#include "constants.h"

NetworkSocket init_udp_network(int argc, char *argv[], int *pid_tcp_handler) {
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
    networkSocket.pid_tcp_handler = &pid_tcp_handler;
    networkSocket.udpSocket.sockfd = sockfd;
    networkSocket.udpSocket.serv_addr = serv_addr;
    networkSocket.udpSocket.network_init = false;

    // Faire le ping pour vérifier que le serveur est bien là
    NetMessage message;
    message.type = NET_REQ_PING;
    if(send_udp_message(&networkSocket.udpSocket, &message).type != NET_REQ_PING) {
        logs(L_INFO, "Network | Error the server is not responding");
        printf("Network | Error the server is not responding\n");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Network initialized");
    networkSocket.udpSocket.network_init = true;

    logs(L_INFO, "Network | Return socket data info");

    return networkSocket;
}

NetMessage send_udp_message(UDPSocketData *udpSocket, NetMessage *message) {
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
                if (!udpSocket->network_init) {
                    printf("Timeout, try %d/%d...\r", 1+nb_try, NET_MAX_TRIES);
                    fflush(stdout);
                }
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
            } else if (message->type == UDP_REQ_PARTIE_LIST && response.type == UDP_REQ_PARTIE_LIST) {
                logs(L_INFO, "Network | Partie Liste received");
                received = true;
            } else if (message->type == UDP_REQ_MAP_LIST && response.type == UDP_REQ_MAP_LIST) {
                logs(L_INFO, "Network | Partie Liste received");
                received = true;
            } else if (message->type == UDP_REQ_CREATE_PARTIE && response.type == UDP_REQ_CREATE_PARTIE) {
                logs(L_INFO, "Network | Partie Liste received");
                received = true;
            } else if (message->type == UDP_REQ_WAITLIST_PARTIE && response.type == UDP_REQ_WAITLIST_PARTIE) {
                logs(L_INFO, "Network | Partie Liste received");
                received = true;
            } else {
                logs(L_INFO, "Network | Unknown message received");
                received = false;
                if (nb_try >= NET_MAX_TRIES) {
                    logs(L_INFO, "Network | Max tries reached");
                    printf("Max tries reached, exiting...\n");
                    exit(EXIT_FAILURE);
                }
                nb_try++;
            }
        }
    }

    return response;
}

void close_tcp_sighandler(int signum) {
    logs(L_INFO, "Network (TCP) | Network closed");
    // TODO : Fermer proprement le réseau
}

void close_udp_sighandler(int signum) {
    logs(L_INFO, "Network (UDP) | Network closed");
    // TODO : Fermer proprement le réseau
}

int wait_for_tcp_connection(GameInterface *gameI, int tcpPort) {
    if (tcpPort == -1) {
        // On écoute en UDP pour attendre le port donné par le serveur

        int pid = fork();

        if (pid == 0) {
            // Child process

            // Modifier le timeout en le supprimant
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            if (setsockopt(gameI->netSocket->udpSocket.sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                logs(L_INFO, "Network | Error setting timeout");
                perror("Error setting timeout");
                exit(EXIT_FAILURE);
            }

            // TODO : handler sigaction sur SIGINT pour fermer proprement le réseau

            // Attendre le port TCP directement sur le socket UDP
            //if(recvfrom(udpSocket->sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {
            

        } else if (pid > 0) {
            // Parent process
            
            // Save the child pid
            **(gameI->netSocket->pid_tcp_handler) = pid;

            return pid;
        } else {
            // Erreur
            perror("Error creating child process");
            logs(L_INFO, "Network | Error creating child process");
            exit(EXIT_FAILURE);
        }
    } else {
        // TODO
    }


    // Si tcpPort == -1 alors on écoute en UDP pour attendre le port donné par le serveur

    return 0;
}

int init_tcp_network(GameInterface *gameI, int port) {
    // TODO
    return 0;
}

void close_tcp_network(TCPSocketData *tcpSocket) {
    // TODO
}

void close_udp_network(UDPSocketData *udpSocket) {
    // TODO
}

NetMessage send_tcp_message(TCPSocketData *tcpSocket, NetMessage *message) {
    // TODO
    NetMessage response = {0};

    return response;
}