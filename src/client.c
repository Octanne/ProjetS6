
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "level.h"
#include "utils.h"
#include "constants.h"

#include "client_gui.h"
#include "client_network.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

int pid_network;

/**
 * @brief Function runned when the game is stopped
 */
void stop_game() {
	// Logs
	logs(L_INFO, "Main | Stopping game...");

	// Free the game interface
    stop_gui();
}

/**
 * @brief Function managing the keyboard events.
 * Running indefinitely until the user press the key to quit the game
 */
void control_handler() {
    int ch;
    while((ch = getch()) != KEY_QUIT_GAME) {
        control_handler_gui(ch);
    }
}

/**
 * @brief Exit function called at the exit of the program
 */
void main_exit() {
    stop_game();
    kill(pid_network, SIGINT);
    waitpid(pid_network, NULL, 0);

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

    // Init gui
    init_gui();

    // Launch control handler
    control_handler();

	return EXIT_SUCCESS;
}

