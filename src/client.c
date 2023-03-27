
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

GameInterface gameInfo;

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

    // Close logs
    closeLogs();
}

/**
 * @brief Main function
 * @return EXIT_SUCCESS if the program exit correctly
 */
int main(int argc, char *argv[]) {

    // Register exit function
    atexit(main_exit);

    // Network initialization
    gameInfo.netSocket = init_udp_network(argc, argv); // Non bloquant

    // Graphics updater & control handler
    init_gui_process(&gameInfo); // Bloquant (wait for the gui to close) press Q to close

	return EXIT_SUCCESS;
}

