
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "utils.h"
#include "constants.h"

#include "client_network.h"
#include "gui_process.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

int* pid_tcp_network;

/**
 * @brief Function runned when the game is stopped
 */
void stop_game() {
	// Logs
	logs(L_INFO, "Main | Stopping game...");
}

/**
 * @brief Exit function called at the exit of the program
 */
void main_exit() {
    stop_game();

    // check if the network is still running
    logs(L_INFO, "Main | Check tcp network...");
    if (*pid_tcp_network != 0 && kill(*pid_tcp_network, 0) == 0) {
        logs(L_INFO, "Main | Stopping tcp network...");
        kill(*pid_tcp_network, SIGINT);
        waitpid(*pid_tcp_network, NULL, 0);
        // free the pid
        *pid_tcp_network = 0;
        free(pid_tcp_network);
    }

    // Close logs
    closeLogs();
}

/**
 * @brief Main function
 * @return EXIT_SUCCESS if the program exit correctly
 */
int main(int argc, char *argv[]) {
    // Init pid pointer for the network
    pid_tcp_network = malloc(sizeof(int));

    // Network init
    NetworkSocket netSocket = init_udp_network(argc, argv, pid_tcp_network); // Non bloquant

    // Register exit function
    atexit(main_exit);

    // Graphics updater & control handler
    init_gui_process(&netSocket); // Bloquant (wait for the gui to close) press Q to close

    // Close the network
    close_udp_network(&netSocket);

	return EXIT_SUCCESS;
}

