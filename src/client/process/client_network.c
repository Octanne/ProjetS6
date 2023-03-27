
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

/**
 * @brief Signal handler
 * 
 * @param signo		int signal number
 */
void sig_usr(int signo) {
	if (signo == SIGUSR1) {
		logs(L_INFO, "Network | Message readable from waitlist");
	}
}

/**
 * @brief Init UDP socket and return it
 * 
 * @param argc	Number of arguments
 * @param argv	Arguments
 * 
 * @return NetworkSocket 
 */
NetworkSocket init_udp_network(int argc, char *argv[]) {

	// Logs and variables init
	logs(L_INFO, "Network | Init network...");
	int opt;
	int port = 0;
	char *host = NULL;

	// Switch case on the arguments (-p and -h) to get the port and the host
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

	// Check if the port and the host are set
	if (port == 0 || host == NULL) {
		fprintf(stderr, "Les deux options -p et -h sont obligatoires\n");
		exit(EXIT_FAILURE);
	}

	// Logs the port and the host
	logs(L_INFO, "Network | Port : %d, Host : %s", port, host);
	

	// Création de la socket avec vérification
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		logs(L_INFO, "Network | Socket creation error");
		exit(EXIT_FAILURE);
	}

	// Initialisation de la structure de l'adresse du serveur
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, host, &serv_addr.sin_addr) != 1) {
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
	networkSocket.pid_receive_tcp = 0;
	networkSocket.pid_waitlist = 0;
	networkSocket.udpSocket.waitlist_running = false;
	networkSocket.udpSocket.sockfd = sockfd;
	networkSocket.udpSocket.serv_addr = serv_addr;
	networkSocket.udpSocket.network_init = false;

	// Faire le ping pour vérifier que le serveur est bien là
	NetMessage message;
	message.type = NET_REQ_PING;
	if (send_udp_message(&networkSocket.udpSocket, &message).type != NET_REQ_PING) {
		logs(L_INFO, "Network | Error the server is not responding");
		printf("Network | Error the server is not responding\n");
		exit(EXIT_FAILURE);
	}

	// Logs and return
	logs(L_INFO, "Network | Network initialized");
	networkSocket.udpSocket.network_init = true;
	logs(L_INFO, "Network | Return socket data info");
	return networkSocket;
}

/**
 * @brief Send UDP message through the socket and return the response
 * 
 * @param udpSocket		UDPSocketData
 * @param message		NetMessage to send
 * 
 * @return NetMessage 
 */
NetMessage send_udp_message(UDPSocketData *udpSocket, NetMessage *message) {

	// Variables init
	int nb_try = 0;
	NetMessage response;
	bool received = false;

	// While the message is not received and the number of tries is not reached
	while (!received && nb_try < NET_MAX_TRIES) {

		// Send message
		if (sendto(udpSocket->sockfd, message, sizeof(NetMessage), 0, (struct sockaddr*)&udpSocket->serv_addr, sizeof(struct sockaddr_in)) == -1) {
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
		if (!udpSocket->waitlist_running && recvfrom(udpSocket->sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {

			// Error handling
			if (errno == EINTR) {
				logs(L_INFO, "Network | Network closed while receiving response");
				exit(EXIT_SUCCESS);
			} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				logs(L_INFO, "Network | Timeout");
				if (!udpSocket->network_init) {
					printf("Timeout, try %d/%d...\r", 1 + nb_try, NET_MAX_TRIES);
					fflush(stdout);
				}
				nb_try++;
			} else {
				perror("Error receiving response");
				logs(L_INFO, "Network | Error receiving response %d", errno);
				exit(EXIT_FAILURE);
			}
		}
		// Else, the message is received
		else {
			// If the waitlist is running
			if (udpSocket->waitlist_running) {
				logs(L_INFO, "Network | Waitlist is running, waiting for response");

				// Wait for the response (a thread will set udpSocket->waitlist_message and wake up this thread)
				pause();

				// Check if the response is NULL
				if (udpSocket->waitlist_message.type == -1) {
					logs(L_INFO, "Network | Error receiving response (waitlist_message is NULL (type = -1))");
					exit(EXIT_FAILURE);
				}

				// Copy udpSocket->waitlist_message to response
				response = udpSocket->waitlist_message;
			}

			// Logs
			logs(L_INFO, "Network | Message received");
			logs(L_INFO, "Network | Message type : %d", message->type);

			// Check if the response is the expected one
			if (message->type == response.type) {
				received = true;
			
				// If elses for each message type
				if (message->type == NET_REQ_PING)
					logs(L_INFO, "Network | Ping received");
				else if (message->type >= UDP_REQ_PARTIE_LIST && message->type <= UDP_REQ_WAITLIST_PARTIE)
					logs(L_INFO, "Network | Partie Liste received");
			}
			// Else, the message is not the expected one
			else {
				logs(L_INFO, "Network | Unknown message received");
				received = false;
				if (nb_try++ >= NET_MAX_TRIES) {
					logs(L_INFO, "Network | Max tries reached");
					printf("Max tries reached, exiting...\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	return response;
}

/**
 * @brief Stop waitlist thread by exiting it
 */
void stop_waitlist() {
	logs(L_INFO, "Network | Stop waitlist thread");
	pthread_exit(NULL);
}

/**
 * @brief Thread waitlist : attend la réponse du serveur pour la partie
 * 
 * @param arg	GameInterface pointer
*/
void* waitlist_handler(void *arg) {

	// Sig action sur le threads pour pouvoir le kill
	struct sigaction sa;
	sa.sa_handler = stop_waitlist;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	GameInterface *gameInfo = (GameInterface*)arg;

	// Set waitlist_message to NULL equivalent
	gameInfo->netSocket.udpSocket.waitlist_message.type = -1;

	while (1) {

		// On attend la réponse du serveur UDP
		NetMessage response;
		if (recvfrom(gameInfo->netSocket.udpSocket.sockfd, &response, sizeof(response), 0, NULL, 0) == -1) {

			// Error handling
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
		}
		// On a reçu une réponse
		else {
			logs(L_INFO, "Network | Waitlist Message received");

			// If the response is a waitlist message and the waitState is true
			if (response.type == UDP_REQ_WAITLIST_PARTIE && response.partieWaitListMessage.waitState) {
				logs(L_INFO, "Network | Waitlist received");

				// If the port is -1, exit the thread
				if (response.partieWaitListMessage.portTCP == -1) {
					stop_waitlist();
				}

				// On initialise la connexion TCP
				if (init_tcp_network(gameInfo, response.partieWaitListMessage.portTCP) == -1) {
					logs(L_INFO, "Network | Error initializing TCP network");
					exit(EXIT_FAILURE);
				}

				// On quitte le waitMode et on stop le thread
				gameInfo->menuInfo.waitToJoin = false;
				stop_waitlist();
			}
			// Else, we save the message for the main thread
			else {

				// Enregistrement du message pour le threads principal.
				gameInfo->netSocket.udpSocket.waitlist_message = response;

				// Send signal to main thread
				kill(getpid(), SIGUSR1);
				logs(L_INFO, "Network | Enregistrement du message pour le threads principal"); 
			}
		}
	}

	return NULL;
}

/** 
 * @brief Débute la lecture de la socket UDP pour le message de connexion TCP, Elle ouvre son propre processus.
 * 
 * @param gameI		GameInterface
 * @param tcpPort	TCP port to connect to
*/
void wait_tcp_connection(GameInterface *gameI, int tcpPort) {

	if (tcpPort == -1) {
		// On écoute en UDP pour attendre le port donné par le serveur
		// Démarrer le thread d'écoute
		gameI->netSocket.udpSocket.waitlist_running = true;
		pthread_create(&gameI->netSocket.pid_waitlist, NULL, waitlist_handler, gameI);
	}
	else {
		// On initialise la connexion TCP
		gameI->menuInfo.waitToJoin = false;
		if (init_tcp_network(gameI, tcpPort) == -1) {
			logs(L_INFO, "Network | Error initializing TCP network");
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * @brief Stop reading UDP socket for TCP connection message
 * 
 * @param gameI	GameInterface
 */
void stop_wait_tcp_connection(GameInterface *gameI) {

	// Logs
	logs(L_INFO, "Network | Stopping wait tcp connection thread...");

	// Check if waitlist is running
	if (gameI->netSocket.udpSocket.waitlist_running) {

		// Kill waitlist thread
		pthread_kill(gameI->netSocket.pid_waitlist, SIGINT);
		pthread_join(gameI->netSocket.pid_waitlist, NULL);
		gameI->netSocket.udpSocket.waitlist_running = false;
	}
}

/**
 * @brief Stop reading TCP socket
 * 
 * @param gameI	GameInterface
 */
void stop_read_tcp_socket(GameInterface *gameI) {

	// Check if waitlist is running
	if (gameI->netSocket.tcpSocket.read_running) {

		// Kill waitlist thread
		pthread_kill(gameI->netSocket.pid_receive_tcp, SIGINT);
		pthread_join(gameI->netSocket.pid_receive_tcp, NULL);
	}
}

/**
 * @brief TCP read thread
 * 
 * @param arg	GameInterface pointer
 */
void* tcp_read_handler(void *arg) {
	GameInterface *gameInfo = (GameInterface*)arg;

	// While the thread is running
	while (gameInfo->netSocket.tcpSocket.read_running) {
		// TODO read from socket
		logs(L_INFO, "Network | TCP | tcp read thread running...");
		sleep(2);
	}

	// Logs and exit
	logs(L_INFO, "Network | TCP | tcp read thread stopped!");
	return NULL;
}

/**
 * @brief Init TCP socket and put data in the GameInterface
 * 
 * @param gameI		GameInterface
 * @param tcpPort	TCP port to connect to
 * 
 * @return int (0 if success, -1 if error)
 */
int init_tcp_network(GameInterface *gameI, int port) {

	// Init socket
	logs(L_INFO, "Network | TCP | Init TCP network...");
	if ((gameI->netSocket.tcpSocket.sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("Error creating socket");
		logs(L_INFO, "Network | TCP | Error creating socket : %s", strerror(errno));
		return -1;
	}

	// Init server address
	gameI->netSocket.tcpSocket.serv_addr.sin_family = AF_INET;
	gameI->netSocket.tcpSocket.serv_addr.sin_port = htons(port);
	gameI->netSocket.tcpSocket.serv_addr.sin_addr.s_addr = gameI->netSocket.udpSocket.serv_addr.sin_addr.s_addr;

	// Connect to server
	if (connect(gameI->netSocket.tcpSocket.sockfd, (struct sockaddr *)&gameI->netSocket.tcpSocket.serv_addr, sizeof(gameI->netSocket.tcpSocket.serv_addr)) == -1) {
		perror("Error connecting to server");
		logs(L_INFO, "Network | TCP | Error connecting to server : %s", strerror(errno));
		return -1;
	}

	// Create read thread for TCP socket
	gameI->netSocket.tcpSocket.read_running = true;
	if (pthread_create(&gameI->netSocket.pid_receive_tcp, NULL, tcp_read_handler, gameI) != 0) {
		perror("Error creating read thread");
		logs(L_INFO, "Network | TCP | Error creating read thread : %s", strerror(errno));
		return -1;
	}

	// Logs and return
	logs(L_INFO, "Network | Init TCP network done!");
	return 0;
}

/**
 * @brief Close TCP socket
 * 
 * @param gameI	GameInterface
 */
void close_tcp_socket(GameInterface *gameI) {

	// Error handling
	if (close(gameI->netSocket.tcpSocket.sockfd) == -1) {
		perror("Error closing socket");
		exit(EXIT_FAILURE);
	}

	logs(L_INFO, "Network | TCP | Socket closed!");
}

/**
 * @brief Send TCP message to server through the socket
 * 
 * @param tcpSocket		TCPSocketData
 * @param message		NetMessage to send
 * 
 * @return NetMessage 
 */
NetMessage send_tcp_message(TCPSocketData *tcpSocket, NetMessage *message) {
	// TODO sendMessage
	NetMessage response = {0};

	return response;
}

