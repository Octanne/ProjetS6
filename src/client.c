
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

#include "level.h"
#include "utils.h"
#include "constants.h"

#include "level_update.h"
#include "client_gui.h"

#include "client_network.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

Level level;
int pid_network;

/**
 * @brief Function to clear the level and generate a new one
 */
void clear_level() {
	// Free level and generate a new one
    level_free(&level);
    level = levelCreer();

	// Logs and refresh level
    logs(L_INFO, "Main | Level cleared");
    logs(L_INFO, "Main | Level value : %X", level);
    refresh_level(level);
}

/**
 * @brief Function to load a level and save the old one
 */
void load_level(int newLevel) {
    // TODO to change
    level_free(&level);
    level = levelCreer();

    // Logs and refresh level
    logs(L_INFO, "Main | New level load : %d", newLevel);
    logs(L_INFO, "Main | Level %d : %d items loaded", newLevel, level.listeObjet.taille);
    set_text_info("Level loaded", 1, GREEN_COLOR);

    refresh_level(level);
}

/**
 * @brief Function runned when the game is stopped
 */
void stop_game() {
	// Logs
	logs(L_INFO, "Main | Stopping game...");

	// Free the game interface
    stop_gui();

	// Free the level
    level_free(&level);

	// Close logs
    closeLogs();
}

/**
 * @brief Function managing the mouse event on the tools window
 * 
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_toolsWindow(short posX, short posY) {

	// Check if the mouse is inside the tools window
    if (posX >= 62 && posX < 77 && posY >= 0 && posY < 20) {
        // If we want to do something with the tools
    }
}

/**
 * @brief Function managing the mouse event on the level window
 * 
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_levelWindow(short posX, short posY) {
	// Logs
    logs(L_INFO, "Main | Level value : %X", level);

	// Check if the mouse is inside the level window
    if (posX >= 0 && posX < MATRICE_LEVEL_X && posY >= 0 && posY < MATRICE_LEVEL_Y) {
        // If we want to do something with the level
    }
}

/**
 * @brief Function managing the mouse events
 * 
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_event(short posX, short posY) {
    // convert to window level coordinates
    posX -= 1;
    posY -= 1;

    // Apply the mouse event on the level window
    mouse_levelWindow(posX, posY);

    // Apply the mouse event on the tools window
    mouse_toolsWindow(posX, posY);

    // Write down mouse position
    char text[100];
    sprintf(text, "Position : (Y, X) -> (%i, %i)", posY, posX);
    set_text_info(text, 2, LBLUE_COLOR);
}

/**
 * @brief Function managing the keyboard events.
 * Running indefinitely until the user press the key to quit the game
 */
void control_handler() {
    int ch;
	int posX, posY;
    while((ch = getch()) != KEY_QUIT_GAME) {
        switch (ch) {

            case KEY_UP:
				// Write down the action
                set_text_info("Action: UP", 1, GREEN_COLOR);
            break;
			
            case KEY_DOWN:
				// Write down the action
                set_text_info("Action: DOWN", 1, GREEN_COLOR);
            break;

            case KEY_LEFT:
				// Write down the action
                set_text_info("Action: LEFT", 1, GREEN_COLOR);
            break;

            case KEY_RIGHT:
				// Write down the action
                set_text_info("Action: RIGHT", 1, GREEN_COLOR);
            break;

            case KEY_VALIDATE:
				// Write down the action
                set_text_info("Action: VALIDATE", 1, GREEN_COLOR);
            break;

            case KEY_MOUSE:
				// Mouse event handler
                if (mouse_getpos(&posX, &posY) == OK)
                    mouse_event((short)posX, (short)posY);
            break;
        }
    }
}

/**
 * @brief Exit function called at the exit of the program
 */
void main_exit() {
    stop_game();
    kill(pid_network, SIGKILL);
}

/**
 * @brief Main function
 * @return EXIT_SUCCESS if the program exit correctly
 */
int main(int argc, char *argv[]) {
    // Register exit function
    atexit(main_exit);

    // Network init
    pid_network = init_network(argc, argv);

    // Init gui
    init_gui();

    // Load level
    load_level(1);

    // Launch control handler
    control_handler();

    // Stop network
    kill(pid_network, SIGKILL);

	return EXIT_SUCCESS;
}

