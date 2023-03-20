
#include "control_handler.h"

#include <locale.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "utils.h"
#include "constants.h"

#include "client_gui.h"
#include "menu_gui.h"
#include "game_gui.h"

extern GameInterface *gameI_global;
extern pthread_mutex_t gameI_global_lock;

/**
 * @brief Function managing the mouse events
 * 
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_event(GameInterface *gameI, short posX, short posY) {
    // convert to window level coordinates
    posX -= 1;
    posY -= 1;

    // Check if the mouse is inside the main window
    if (posX >= 0 && posX < MATRICE_LEVEL_X && posY >= 0 && posY < MATRICE_LEVEL_Y) {
        if (gameI->inMenu == true) {
            menu_mouse_main_window(gameI, posX, posY);
        } else {
            game_mouse_level_window(gameI, posX, posY);
        }
    }

    // Check if the mouse is inside the right window
    else if (posX >= 62 && posX < 77 && posY >= 0 && posY < 20) {
        if (gameI->inMenu == true) {
            menu_mouse_right_menu(gameI, posX, posY);
        } else {
            game_mouse_player_menu(gameI, posX, posY);
        }
    }

    // Write down mouse position
    char text[100];
    sprintf(text, "Position : (Y, X) -> (%i, %i)", posY, posX);
    set_text_info_gui(gameI, text, 2, LBLUE_COLOR);
}

/**
 * @brief Function managing the keyboard events
 * 
 * @param key : character typed
 */
void control_handler_gui(GameInterface *gameI, int key) {
	int posX, posY;
    switch (key) {
        case KEY_MOUSE:
            // Mouse event handler
            if (mouse_getpos(&posX, &posY) == OK)
                mouse_event(gameI, (short)posX, (short)posY);
        break;
        default: 
            if (gameI->inMenu == true) {
                menu_keyboard_handler(gameI, key);
            } else {
                game_keyboard_handler(gameI, key);
            }
        break;
    }
}

/**
 * @brief Function managing the keyboard events.
 * Running indefinitely until the user press the key to quit the game
 */
void *control_handler(void *arg) {
    int ch;
    logs(L_INFO, "Main | Launching control handler...");

    while((ch = getch()) != KEY_QUIT_GAME) {
        // Lock the mutex to access the game interface and check return value
        if (pthread_mutex_lock(&gameI_global_lock) != 0) {
            logs(L_INFO, "Main | Error while locking the mutex!");
            exit(EXIT_FAILURE);
        }
        control_handler_gui(gameI_global, ch);
        // Unlock the mutex to access the game interface and check return value
        if (pthread_mutex_unlock(&gameI_global_lock) != 0) {
            logs(L_INFO, "Main | Error while unlocking the mutex!");
            exit(EXIT_FAILURE);
        }
    }
    logs(L_INFO, "Main | Control handler stopped!");
    kill(getpid(), SIGINT); // On quitte le programme
    pthread_exit(NULL); // On quitte le thread
}
