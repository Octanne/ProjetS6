
#include "serveur_network.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "utils.h"
#include "constants.h"
#include "liste.h"

#include "net_struct.h"

int network_running = true;

void close_network() {
    logs(L_INFO, "Network | Closing network...");
    printf("Network | Closing network...\n");
    network_running = false;
}

int init_network(int argc, char *argv[]) {
    logs(L_INFO, "Network | Init network...");
    printf("Network | Init network...\n");
    int opt;
    int port = 0;
	char* host = NULL;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
			case 'h':
				host = optarg;
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
	UDPSocketData udpSocket;
	// TODO ALEX

    // Create the socket
    if((udpSocket.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error creating socket");
        logs(L_INFO, "Network | Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
	if (host != NULL) {
    	server_address.sin_addr.s_addr = INADDR_ANY;
	} else {
		server_address.sin_addr.s_addr = inet_addr(host);
	}

    // Bind socket
    if(bind(udpSocket.sockfd, (struct sockaddr*)&server_address, sizeof(struct sockaddr_in)) == -1) {
        perror("Error naming socket");
        logs(L_INFO, "Network | Error naming socket");
        exit(EXIT_FAILURE);
    }

    // Logs the port and the host
    logs(L_INFO, "Network | Port : %d, Host : %s", port, inet_ntoa(server_address.sin_addr));
    printf("Network | listening on %s:%d for UDP packets ... \n", inet_ntoa(server_address.sin_addr), port);

    // ouverture d'un processus fils
    int pid = fork();

    if (pid != 0) {
        // processus père
        return pid;
    }

    // register close_network as the handler for SIGINT with sigaction
    struct sigaction sa;
    sa.sa_handler = close_network;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    PartieManager partieManager = partieManager_create(udpSocket);

    // processus réseau
    while (network_running) {
        network_running = udp_request_handler(partieManager.udpSocket.sockfd, &partieManager);
    }

    close(sockfd);

    printf("Network | Network processus closed\n");
    logs(L_INFO, "Network | Network processus closed");

    exit(EXIT_SUCCESS);
}

bool udp_request_handler(int sockfd, PartieManager *partieManager) {
    bool status = true;
    NetMessage request;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address;

    // Receive the request
    printf("Network | waiting for a request.\n");
    logs(L_INFO, "Network | Waiting for a request");
    if(recvfrom(sockfd, &request, sizeof(request), 0, (struct sockaddr*)&client_address, &addr_len) == -1) {
        if (errno == EINTR) {
            // SIGINT received
            status = false;
            return status;
        }

        perror("Error receiving message");
        logs(L_INFO, "Network | Error receiving message");
        exit(EXIT_FAILURE);
    }

    // Handle the request
    printf("Network | received request %d from %s:%d\n", request.type, 
    inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    logs(L_INFO, "Network | Received request %d from %s:%d", request.type, 
    inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    NetMessage response;

    switch (request.type) {
        case NET_REQ_PING:
            printf("Network | received ping request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received ping request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            response.type = NET_REQ_PING;
            status = true;
            break;
        case UDP_REQ_PARTIE_LIST:
            printf("Network | received partie list request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received partie list request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            response.partieListeMessage = listPartie(partieManager, request.partieListeMessage.numPage);
            response.type = UDP_REQ_PARTIE_LIST;
            status = true;
            break;
        case UDP_REQ_MAP_LIST:
            printf("Network | received map list request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received map list request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            response.mapListMessage = listMaps(partieManager, request.mapListMessage.numPage);
            response.type = UDP_REQ_MAP_LIST;
            status = true;
            break;
        case UDP_REQ_CREATE_PARTIE:
            printf("Network | received create partie request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received create partie request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            response.partieCreateMessage = createPartie(partieManager, request.partieCreateMessage.maxPlayers, 
                request.partieCreateMessage.numMap, client_address);
            response.type = UDP_REQ_CREATE_PARTIE;
            status = true;
            break;
        case UDP_REQ_WAITLIST_PARTIE:
            printf("Network | received waitlist partie request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received waitlist partie request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            response.partieWaitListMessage = waitListePartie(partieManager, request.partieWaitListMessage.numPartie, 
                request.partieWaitListMessage.waitState, client_address);
            response.type = UDP_REQ_WAITLIST_PARTIE;
            status = true;
            break;
        default:
            printf("Network | received unknown request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            logs(L_INFO, "Network | Received unknown request from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            status = true;
            break;
    }
    
    // Send the response
    printf("Network | sending response to %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    logs(L_INFO, "Network | Sending response to %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    if(sendto(sockfd, &response, sizeof(response), 0, (struct sockaddr*)&client_address, sizeof(struct sockaddr_in)) == -1) {
        perror("Error sending response");
        logs(L_INFO, "Network | Error sending response");
        exit(EXIT_FAILURE);
    }

    logs(L_INFO, "Network | Response sent");
    printf("Network | response sent.\n");

    return status;
}

