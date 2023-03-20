
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "utils.h"
#include "constants.h"

#include "client_network.h"
#include "gui_updater.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

int pid_network, pid_updater_graphics;

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
    logs(L_INFO, "Main | Check network...");
    if (kill(pid_network, 0) == 0) {
        logs(L_INFO, "Main | Stopping network...");
        kill(pid_network, SIGINT);
        waitpid(pid_network, NULL, 0);
    }

    // check if the updater is still running
    logs(L_INFO, "Main | Check updater...");
    if (kill(pid_updater_graphics, 0) == 0) {
        logs(L_INFO, "Main | Stopping updater...");
        kill(pid_updater_graphics, SIGINT);
        waitpid(pid_updater_graphics, NULL, 0);
    }

    // Close logs
    closeLogs();
}

/**
 * @brief Main function
 * @return EXIT_SUCCESS if the program exit correctly
 */
int main(int argc, char *argv[]) {
    // Network init
    pid_network = init_network(argc, argv);

    // Register exit function
    atexit(main_exit);

    // Graphics updater & control handler
    pid_updater_graphics = init_updater_gui();

    // Wait for the end of the game
    waitpid(pid_updater_graphics, NULL, 0);

	return EXIT_SUCCESS;
}

