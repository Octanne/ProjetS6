
#include "client_network.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "utils.h"
#include "constants.h"

#include "net_message.h"

#define MESSAGE_QUEUE_KEY "ProjetS6Network"

int pid_net;
int msqid_network;
bool network_running = true;
bool network_init = false;

void close_network() {
    logs(L_INFO, "Network | Closing network...");
    network_running = false;

    // Close the message queue
    if (msgctl(msqid_network, IPC_RMID, NULL) == -1) {
        logs(L_INFO, "Network | Error while closing the message queue");
        exit(EXIT_FAILURE);
    }
}

int init_network(int argc, char *argv[]) {
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

    // Création de la file de message
    key_t key = ftok(MESSAGE_QUEUE_KEY, 1);
    if ((msqid_network = msgget(key, IPC_CREAT | 0666)) < 0) {
        logs(L_INFO, "Network | Error creating message queue");
        perror("Error creating message queue");
        exit(EXIT_FAILURE);
    }

    // Faire le ping pour vérifier que le serveur est bien là
    NetMessage message;
    message.type = NET_REQ_PING;
    if(!process_udp_message(&message, sockfd, serv_addr)) {
        logs(L_INFO, "Network | Error the server is not responding");
        printf("Network | Error the server is not responding\n");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Network initialized");

    // ouverture d'un processus fils
    pid_net = fork();

    if (pid_net != 0) {
        // processus père
        return pid_net;
    }

    // Signal handler via sigaction
    struct sigaction sa;
    sa.sa_handler = close_network;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    network_init = true;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        logs(L_INFO, "Network | Error setting signal handler");
        exit(EXIT_FAILURE);
    }

    while (network_running) {
        network_running = udp_message_handler(sockfd, serv_addr);
    }

    close_network();
    logs(L_INFO, "Network | Network closed");

    exit(EXIT_SUCCESS);
}

bool add_udp_message_to_queue(NetMessage *message) {
    // Use message queue to send message
    if (msgsnd(msqid_network, message, sizeof(NetMessage), 0) < 0) {
        logs(L_INFO, "Network | Error sending message to queue");
        perror("Error sending message to queue");
        exit(EXIT_FAILURE);
    }
    return true;
}

NetMessage *get_udp_message_from_queue() {
    // Use message queue to get message
    NetMessage *message = malloc(sizeof(NetMessage));
    if (msgrcv(msqid_network, message, sizeof(NetMessage), 0, 0) < 0) {
        // Si interruption du programme, on quitte proprement
        if (errno == EINTR) {
            logs(L_INFO, "Network | Network closed by signal");
            exit(EXIT_SUCCESS);
        }

        logs(L_INFO, "Network | Error getting message from queue");
        perror("Error getting message from queue");
        exit(EXIT_FAILURE);
    }
    return message;
}

bool process_udp_message(NetMessage *message, int sockfd, struct sockaddr_in serv_addr) {
    bool received = false;
    int nb_try = 0;
    while (!received && nb_try < NET_MAX_TRIES) {
        // Send message
        if(sendto(sockfd, message, sizeof(NetMessage), 0, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) == -1) {
            if (errno == EINTR) {
                logs(L_INFO, "Network | Network closed by signal");
                exit(EXIT_SUCCESS);
            }
            perror("Error sending message");
            logs(L_INFO, "Network | Error sending message");
            exit(EXIT_FAILURE);
        }
        logs(L_INFO, "Network | Message sent");

        // Receive response
        NetMessage response;
        if(recvfrom(sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {
            if (errno == EINTR) {
                logs(L_INFO, "Network | Network closed by signal");
                exit(EXIT_SUCCESS);
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                logs(L_INFO, "Network | Timeout");
                if (!network_init) printf("Timeout, try %d/%d...\r", 1+nb_try, NET_MAX_TRIES);
                fflush(stdout);
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

    return received;
}

bool udp_message_handler(int sockfd, struct sockaddr_in serv_addr) {
    // Get message from queue
    NetMessage *message = get_udp_message_from_queue();
    
    // Send message
    bool received = process_udp_message(message, sockfd, serv_addr);

    // Free message
    free(message);

    return received;
}

