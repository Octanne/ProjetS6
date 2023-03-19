
#include "serveur_network.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
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

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (port == 0) {
        fprintf(stderr, "Les deux options -p et -h sont obligatoires\n");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_address;

    // Create the socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error creating socket");
        logs(L_INFO, "Network | Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset((char *) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(struct sockaddr_in)) == -1) {
        perror("Error naming socket");
        logs(L_INFO, "Network | Error naming socket");
        exit(EXIT_FAILURE);
    }

    // Logs the port and the host
    logs(L_INFO, "Network | Port : %d, Host : %s", port);
    printf("Server: listening on port %d for UDP packets ... \n", port);

    // ouverture d'un processus fils
    int pid = fork();

    if (pid != 0) {
        // processus père
        return pid;
    }

    // processus réseau
    while (isRun) {
        udp_request_handler(sockfd);
    }

    close(sockfd);
    return 0;
}

int udp_request_handler(int sockfd) {
    NetMessage request;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address;

    // Receive the request
    printf("Server: waiting for a request.\n");
    logs(L_INFO, "Network | Waiting for a request");
    if(recvfrom(sockfd, &request, sizeof(request), 0, (struct sockaddr*)&client_address, &addr_len) == -1) {
        perror("Error receiving message");
        logs(L_INFO, "Network | Error receiving message");
        exit(EXIT_FAILURE);
    }

    // Handle the request
    printf("Server: received request %d from %s:%d\n", request.type, 
    inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    logs(L_INFO, "Network | Received request %d from %s:%d", request.type, 
    inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    NetMessage response;

    switch (request.type) {
        case NET_REQ_PING:
            printf("Server: received ping request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received ping request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            response.type = NET_REQ_PING;
            break;
        // TODO add other requests
        default:
            printf("Server: received unknown request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received unknown request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            break;
    }
    
    // Send the response
    printf("Server: sending response to %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    logs(L_INFO, "Network | Sending response to %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    if(sendto(sockfd, &response, sizeof(response), 0, (struct sockaddr*)&client_address, sizeof(struct sockaddr_in)) == -1) {
        perror("Error sending response");
        logs(L_INFO, "Network | Error sending response");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Response sent");
    printf("Server: response sent.\n");

    return 0;
}

