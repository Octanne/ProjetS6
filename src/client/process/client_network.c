
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

void sig_usr(int signo) {
    if (signo == SIGUSR1) {
        logs(L_INFO, "Network | Message readable from waitlist");
    }
}

NetworkSocket *init_udp_network(int argc, char *argv[]) {
    logs(L_INFO, "Network | Init network...");
    int opt;
    int port = 0;
    char *host = NULL;

    while ((opt = getopt(argc, argv, "p:h:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s <-p port> <-h host>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // SigAction
    struct sigaction sa;
    sa.sa_handler = sig_usr;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        logs(L_INFO, "Network | Sigaction error");
        exit(EXIT_FAILURE);
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
    NetworkSocket *networkSocket = malloc(sizeof(NetworkSocket));
    networkSocket->pid_receive_tcp = 0;
    networkSocket->pid_waitlist = 0;
    networkSocket->udpSocket.waitlist_running = false;
    networkSocket->udpSocket.sockfd = sockfd;
    networkSocket->udpSocket.serv_addr = serv_addr;
    networkSocket->udpSocket.network_init = false;

    // Faire le ping pour vérifier que le serveur est bien là
    NetMessage message;
    message.type = NET_REQ_PING;
    if(send_udp_message(&networkSocket->udpSocket, &message).type != NET_REQ_PING) {
        logs(L_INFO, "Network | Error the server is not responding");
        printf("Network | Error the server is not responding\n");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Network initialized");
    networkSocket->udpSocket.network_init = true;

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
        if(!udpSocket->waitlist_running && recvfrom(udpSocket->sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {
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
                logs(L_INFO, "Network | Error receiving response %d", errno);
                exit(EXIT_FAILURE);
            }
        } else {
            if (udpSocket->waitlist_running) {
                logs(L_INFO, "Network | Waitlist is running, waiting for response");
                pause();

                if (udpSocket->waitlist_message == NULL) {
                    logs(L_INFO, "Network | Error receiving response (waitlist_message is NULL)");
                    exit(EXIT_FAILURE);
                }

                // Copy udpSocket->waitlist_message to response
                memcpy(&response, udpSocket->waitlist_message, sizeof(NetMessage));
                // Free udpSocket->waitlist_message
                free(udpSocket->waitlist_message);
            }

            logs(L_INFO, "Network | Message received");

            logs(L_INFO, "Network | Message type : %d", message->type);

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

void stop_waitlist() {
    logs(L_INFO, "Network | Stop waitlist thread");
    pthread_exit(NULL);
}

void *waitlist_handler(void *arg) {
    // Sig action sur le threads pour pouvoir le kill
    struct sigaction sa;
    sa.sa_handler = stop_waitlist;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    extern GameInterface *gameInfo;

    while(1) {
        // On attend la réponse du serveur
        NetMessage response;
        if(recvfrom(gameInfo->netSocket->udpSocket.sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {
            if (errno == EINTR) {
                logs(L_INFO, "Network | Network closed while receiving response");
                exit(EXIT_SUCCESS);
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                logs(L_INFO, "Network | Timeout rewaiting for response");
            } else {
                perror("Error receiving response");
                logs(L_INFO, "Network | Error receiving response");
                exit(EXIT_FAILURE);
            }
        } else {
            logs(L_INFO, "Network | Waitlist Message received");

            if (response.type == UDP_REQ_WAITLIST_PARTIE && response.partieWaitListMessage.waitState) {
                logs(L_INFO, "Network | Waitlist received");

                if (response.partieWaitListMessage.portTCP == -1) {
                    pthread_exit(NULL);
                }

                // On initialise la connexion TCP
                init_tcp_network(gameInfo, response.partieWaitListMessage.portTCP);
                // On quitte le waitMode
                gameInfo->menuInfo.waitToJoin = false;
                // On kill le thread
                pthread_exit(NULL);
            } else {
                // Enregistrement du message pour le threads principal.
                gameInfo->netSocket->udpSocket.waitlist_message = malloc(sizeof(NetMessage));
                memcpy(gameInfo->netSocket->udpSocket.waitlist_message, &response, sizeof(NetMessage));
                // send signal to main thread
                kill(getpid(), SIGUSR1);
                logs(L_INFO, "Network | Enregistrement du message pour le threads principal"); 
            }
        }
    }

}

void wait_tcp_connection(GameInterface *gameI, int tcpPort) {
    if (tcpPort == -1) {
        // On écoute en UDP pour attendre le port donné par le serveur
        // Démarrer le thread d'écoute
        pthread_create(&gameI->netSocket->pid_waitlist, NULL, waitlist_handler, NULL);
        gameI->netSocket->udpSocket.waitlist_running = true;
    } else {
        // On initialise la connexion TCP
        gameI->menuInfo.waitToJoin = false;
        init_tcp_network(gameI, tcpPort);
    }
}

void stop_wait_tcp_connection(GameInterface *gameI) {
    logs(L_INFO, "Network | Stopping wait tcp connection thread...");
    // Check if waitlist is running
    if (gameI->netSocket->udpSocket.waitlist_running) {
        // Kill waitlist thread
        pthread_kill(gameI->netSocket->pid_waitlist, SIGINT);
        pthread_join(gameI->netSocket->pid_waitlist, NULL);
        gameI->netSocket->udpSocket.waitlist_running = false;
    }
}

void stop_read_tcp_socket(GameInterface *gameI) {
    // Check if waitlist is running
    if (gameI->netSocket->tcpSocket.read_running) {
        // Kill waitlist thread
        pthread_kill(gameI->netSocket->pid_receive_tcp, SIGINT);
        pthread_join(gameI->netSocket->pid_receive_tcp, NULL);
    }
}

void stop_tcp_read() {
    logs(L_INFO, "Network | Stopping tcp read thread...");
    extern GameInterface *gameInfo;
    gameInfo->netSocket->tcpSocket.read_running = false;
}

void *tcp_read_handler(void *arg) {
    // Sig action sur le threads pour pouvoir le kill
    struct sigaction sa;
    sa.sa_handler = stop_tcp_read;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    extern GameInterface *gameInfo;
    while (gameInfo->netSocket->tcpSocket.read_running) {
        // TODO read from socket
        logs(L_INFO, "Network | TCP | tcp read thread running...");
        sleep(2);
    }

    logs(L_INFO, "Network | TCP | tcp read thread stopped!");
    pthread_exit(NULL);
}

int init_tcp_network(GameInterface *gameI, int port) {
    logs(L_INFO, "Network | TCP | Init TCP network...");

    // Init socket
    if ((gameI->netSocket->tcpSocket.sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        logs(L_INFO, "Network | TCP | Error creating socket : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Init server address
    gameI->netSocket->tcpSocket.serv_addr.sin_family = AF_INET;
    gameI->netSocket->tcpSocket.serv_addr.sin_port = htons(port);
    gameI->netSocket->tcpSocket.serv_addr.sin_addr.s_addr = gameI->netSocket->udpSocket.serv_addr.sin_addr.s_addr;

    // Connect to server
    if (connect(gameI->netSocket->tcpSocket.sockfd, (struct sockaddr *)&gameI->netSocket->tcpSocket.serv_addr, sizeof(gameI->netSocket->tcpSocket.serv_addr)) == -1) {
        perror("Error connecting to server");
        logs(L_INFO, "Network | TCP | Error connecting to server : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Create read thread
    gameI->netSocket->tcpSocket.read_running = true;
    if (pthread_create(&gameI->netSocket->pid_receive_tcp, NULL, tcp_read_handler, NULL) != 0) {
        perror("Error creating read thread");
        logs(L_INFO, "Network | TCP | Error creating read thread : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Init TCP network done!");
    return 0;
}

void close_tcp_socket(GameInterface *gameI) {
    // Close the socket
    if(close(gameI->netSocket->tcpSocket.sockfd) == -1) {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | TCP | Socket closed!");
}

NetMessage send_tcp_message(TCPSocketData *tcpSocket, NetMessage *message) {
    // TODO sendMessage
    NetMessage response = {0};

    return response;
}