
#include "client_network.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>

#include "utils.h"
#include "constants.h"

#include "net_message.h"

bool isRun = true; // TODO faire un sigaction pour gérer le ctrl+c

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

    // Send request
    NetMessage request;
    request.type = NET_REQ_PING;

    if(sendto(sockfd, &request, sizeof(request), 0, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error sending message");
        logs(L_INFO, "Network | Error sending message");
        exit(EXIT_FAILURE);
    }
    logs(L_INFO, "Network | Message status sent");

    // Receive response
    NetMessage response;
    if(recvfrom(sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {
        perror("Error receiving response");
        logs(L_INFO, "Network | Error receiving response");
        exit(EXIT_FAILURE);
    }
    logs(L_INFO, "Network | Message status received");

    // ouverture d'un processus fils
    int pid = fork();

    if (pid != 0) {
        // processus père
        return pid;
    }

    // Processus gestion creation/join de partie
    // TODO

    while (isRun) {
        // TODO
    }
    
    return 0;
}

