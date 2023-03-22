
#include "gui_process.h"

#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "constants.h"

#include "client_gui.h"
#include "menu_gui.h"
#include "game_gui.h"

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

void init_gui_process(NetworkSocket *netSocket) {
    logs(L_INFO, "GUI Process | Init gui process...");
    
    GameInterface gameI;
    // Add network socket to game interface
    gameI.netSocket = netSocket;
    // Init graphics
    init_gui(&gameI);

    // Control handler
    int ch;
    logs(L_INFO, "Main | Launching control handler...");
    while((ch = getch()) != KEY_QUIT_GAME) {
        control_handler_gui(&gameI, ch);
    }
    logs(L_INFO, "Main | Control handler stopped!");

    // Close graphics
    stop_gui(&gameI);

    logs(L_INFO, "GUI Process | Gui process stopped!");
}

void update_game_gui(GameInterface *gameI, DataUpdateGame *data) {
    // Lock mutex
    pthread_mutex_lock(&gameI->mutex);
    // Update game data
    gameI->gameInfo.player = data->player;
    refresh_player_menu(gameI);
    Level *level = malloc(sizeof(Level));
    *level = convert_bytes_to_level(data->levelBytes, data->sizeLevel);
    load_level(gameI, level);
    // Unlock mutex
    pthread_mutex_unlock(&gameI->mutex);
}

void write_text_info_bar(GameInterface *gameI, DataTextInfoGUI *data) {
    // Lock mutex
    pthread_mutex_lock(&gameI->mutex);
    // Write text
    set_text_info_gui(gameI, data->text, data->line, data->color);
    // Unlock mutex
    pthread_mutex_unlock(&gameI->mutex);
}