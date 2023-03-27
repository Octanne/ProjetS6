
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

/**
 * @brief Function closing the network (change the network_running variable)
*/
void close_network() {
	logs(L_INFO, "Network | Closing network...");
	printf("Network | Closing network...\n");
	network_running = false;
}

/**
 * @brief Function initializing the network
 * 
 * @param argc	Number of arguments
 * @param argv	Arguments
 * 
 * @return int (0 if success, -1 if error)
*/
int init_network(int argc, char *argv[]) {

	// Logs and Init
	logs(L_INFO, "Network | Init network...");
	printf("Network | Init network...\n");
	int opt;
	int port = 0;
	char* host = NULL;

	// Switch case to get the port and the host (host is optional)
	while ((opt = getopt(argc, argv, "p:h:")) != -1) {
		switch (opt) {
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				host = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s <-p port> [<-h address>]\n", argv[0]);
				return -1;
		}
	}

	// Check if the port is set
	if (port == 0) {
		fprintf(stderr, "L'option -p est obligatoire pour indiquer le port\n");
		return -1;
	}

	// Create the socket
	UDPSocketData udpSocket;
	if ((udpSocket.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Error creating socket");
		logs(L_INFO, "Network | Error creating socket");
		return -1;
	}

	// Logs
	logs(L_INFO, "Network | Socket created");
	printf("Network | Socket created\n");

	// Set the server address on given port
	memset(&udpSocket.serv_addr, 0, sizeof(udpSocket.serv_addr));
	udpSocket.serv_addr.sin_family = AF_INET;	// IPv4
	udpSocket.serv_addr.sin_port = htons(port);	// Port
	
	// Logs
	logs(L_INFO, "Network | Socket address set");
	printf("Network | Socket address set\n");

	// Set the host (if not set, set to INADDR_ANY)
	udpSocket.serv_addr.sin_addr.s_addr = host == NULL ? INADDR_ANY : inet_addr(host);
	logs(L_INFO, "Network | Socket address set");
	printf("Network | Socket address set\n");

	// Bind socket
	if (bind(udpSocket.sockfd, (struct sockaddr*)&udpSocket.serv_addr, sizeof(struct sockaddr_in)) == -1) {
		perror("Error naming socket");
		logs(L_INFO, "Network | Error naming socket");
		return -1;
	}

	// Logs the port and the host
	logs(L_INFO, "Network | Port : %d, Host : %s", port, inet_ntoa(udpSocket.serv_addr.sin_addr));
	printf("Network | listening on %s:%d for UDP packets ... \n", inet_ntoa(udpSocket.serv_addr.sin_addr), port);

	// Create a child process to handle the network requests
	// The parent process quit the function with the pid of the child process
	int pid = fork();
	if (pid != 0)
		return pid;

	// Register close_network as the handler for SIGINT with sigaction
	struct sigaction sa;
	sa.sa_handler = close_network;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	// Create the partie manager
	PartieManager partieManager = partieManager_create(udpSocket);

	// Network process : handle requests
	while (network_running)
		network_running = udp_request_handler(partieManager.udpSocket.sockfd, &partieManager);

	// Close the socket
	close(partieManager.udpSocket.sockfd);

	// Logs and exit the child process
	printf("Network | Network processus closed\n");
	logs(L_INFO, "Network | Network processus closed");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Function that print the response details in the server console
 * 
 * @param message	NetMessage to print
*/
void printResponseDetails(NetMessage message) {

	// Show the response details
	printf("Network | =============================\n");
	printf("Network | response type: %d\n", message.type);

	// Switch case on the response type
	switch (message.type) {

		case UDP_REQ_CREATE_PARTIE:
			printf("Network | partie id: %d\n", message.partieCreateMessage.numPartie);
			printf("Network | creation success: %s\n", message.partieCreateMessage.success ? "true" : "false");
			printf("Network | partie port: %d\n", message.partieCreateMessage.serverPortTCP);
			break;
		
		case UDP_REQ_WAITLIST_PARTIE:
			printf("Network | partie id: %d\n", message.partieWaitListMessage.numPartie);
			printf("Network | join success: %s\n", message.partieWaitListMessage.takeInAccount ? "true" : "false");
			printf("Network | partie port: %d\n", message.partieWaitListMessage.portTCP);
			break;
	}
	printf("Network | =============================\n");
}

/**
 * @brief Function that handle UDP requests
 * 
 * @param sockfd		Socket file descriptor
 * @param partieManager	PartieManager pointer
 * 
 * @return 1 if the network processus need to continue, 0 if it need to stop
 */
int udp_request_handler(int sockfd, PartieManager *partieManager) {

	// Variables initialization
	NetMessage request;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	struct sockaddr_in client_address;

	// Receive the request
	printf("Network | waiting for a request.\n");
	logs(L_INFO, "Network | Waiting for a request");
	if (recvfrom(sockfd, &request, sizeof(request), 0, (struct sockaddr*)&client_address, &addr_len) == -1) {

		// Check if the error is due to a SIGINT
		if (errno == EINTR)
			return false;

		// Else, print the error
		perror("Error receiving message");
		logs(L_INFO, "Network | Error receiving message");
		exit(EXIT_FAILURE);
	}

	// Convert the request's host to a string and port for better code readability
	char* host = inet_ntoa(client_address.sin_addr);
	int port = ntohs(client_address.sin_port);

	// Handle the request
	printf("Network | received request %d from %s:%d\n", request.type, host, port);
	logs(L_INFO, "Network | Received request %d from %s:%d", request.type, host, port);

	// Switch case on the request type and prepare the response
	NetMessage response;
	response.type = request.type;
	switch (request.type) {

		case NET_REQ_PING:
			printf("Network | received ping request from %s:%d\n", host, port);
			logs(L_INFO, "Network | Received ping request from %s:%d", host, port);
			break;

		case UDP_REQ_PARTIE_LIST:
			printf("Network | received partie list request from %s:%d\n", host, port);
			logs(L_INFO, "Network | Received partie list request from %s:%d", host, port);
			response.partieListeMessage = listPartie(partieManager, request.partieListeMessage.numPage);
			break;

		case UDP_REQ_MAP_LIST:
			printf("Network | received map list request from %s:%d\n", host, port);
			logs(L_INFO, "Network | Received map list request from %s:%d", host, port);
			response.mapListMessage = listMaps(partieManager, request.mapListMessage.numPage);
			break;

		case UDP_REQ_CREATE_PARTIE:
			printf("Network | received create partie request from %s:%d\n", host, port);
			logs(L_INFO, "Network | Received create partie request from %s:%d", host, port);
			response.partieCreateMessage = createPartie(partieManager, request.partieCreateMessage.maxPlayers, 
				request.partieCreateMessage.numMap, client_address);
			break;

		case UDP_REQ_WAITLIST_PARTIE:
			printf("Network | received waitlist partie request from %s:%d\n", host, port);
			logs(L_INFO, "Network | Received waitlist partie request from %s:%d", host, port);
			response.partieWaitListMessage = waitListePartie(partieManager, request.partieWaitListMessage.numPartie, 
				request.partieWaitListMessage.waitState, client_address);
			break;

		default:
			printf("Network | received unknown request from %s:%d\n", host, port);
			logs(L_INFO, "Network | Received unknown request from %s:%d", host, port);
			break;
	}
	
	// Print the response details
	printResponseDetails(response);

	// Send the response
	printf("Network | sending response to %s:%d\n", host, port);
	logs(L_INFO, "Network | Sending response to %s:%d", host, port);
	if (sendto(sockfd, &response, sizeof(response), 0, (struct sockaddr*)&client_address, sizeof(struct sockaddr_in)) == -1) {
		perror("Error sending response");
		logs(L_INFO, "Network | Error sending response");
		exit(EXIT_FAILURE);
	}

	// Logs and return
	logs(L_INFO, "Network | Response sent");
	printf("Network | response sent.\n");
	return true;
}

