
#include "client_gui.h"

#include <locale.h>

#include "utils.h"
#include "constants.h"
#include "gui_struct.h"

#include "menu_gui.h"
#include "game_gui.h"

GameInterface gameInterface;

/**
 * @brief Init ncurses windows.
 */
void init_gui() {
    logs(L_INFO, "Main | Ncurses windows init...");
    setlocale(LC_ALL, "");
    ncurses_init();
    ncurses_init_mouse();
    ncurses_colors();
    palette();
    getmaxyx(stdscr, LINES, COLS);
    logs(L_INFO, "Main | Ncurses windows init success!");

    gen_right_menu_gui();
    gen_main_gui();

    if (gameInterface.inMenu == true) {
        menu_init_gui();
    } else {
        game_init_gui();
    }
}

/**
 * @brief Stop ncurses windows and free memory.
 */
void stop_gui() {
    delwin(gameInterface.gui.winMAIN);
    delwin(gameInterface.gui.winTOOLS);
    delwin(gameInterface.gui.winINFOS);
    delwin(gameInterface.gui.cwinMAIN);
    delwin(gameInterface.gui.cwinTOOLS);
    delwin(gameInterface.gui.cwinINFOS);

    ncurses_stop();
    logs(L_INFO, "Main | Ncurses windows deleted!");

    if (gameInterface.inMenu == true) {
        menu_stop_gui();
    } else {
        game_stop_gui();
    }
}

void gen_main_gui() {
    // Main window
    gameInterface.gui.cwinMAIN = newwin(22, 62, 0, 0);
    box(gameInterface.gui.cwinMAIN, 0, 0);
    wrefresh(gameInterface.gui.cwinMAIN);

	// Create a subwindow using derwin
    gameInterface.gui.winMAIN = derwin(gameInterface.gui.cwinMAIN, MATRICE_LEVEL_Y, MATRICE_LEVEL_X, 1, 1);
    wrefresh(gameInterface.gui.winMAIN);

    // Right window
    gameInterface.gui.cwinTOOLS = newwin(22, 15, 0, 62);
    box(gameInterface.gui.cwinTOOLS, 0, 0);
    wmove(gameInterface.gui.cwinTOOLS, 0, 0);
    //wprintw(gameInterface.gui.cwinTOOLS, " Tools ");
    wrefresh(gameInterface.gui.cwinTOOLS);

	// Create a subwindow using derwin
    gameInterface.gui.winTOOLS = derwin(gameInterface.gui.cwinTOOLS, 20, 13, 1, 1);
    wrefresh(gameInterface.gui.winTOOLS);

    // Informations window
    gameInterface.gui.cwinINFOS = newwin(5, 77, 22, 0);
    box(gameInterface.gui.cwinINFOS, 0, 0);
    wmove(gameInterface.gui.cwinINFOS, 0, 0);
    wprintw(gameInterface.gui.cwinINFOS, " Informations ");
    wrefresh(gameInterface.gui.cwinINFOS);

	// Create a subwindow using derwin
    gameInterface.gui.winINFOS = derwin(gameInterface.gui.cwinINFOS, 3, 75, 1, 1);

	// Set text info and refresh window
    set_text_info_gui("Press 'Q' to quit...", 0, RED_COLOR);
    wrefresh(gameInterface.gui.winINFOS);
}

void gen_right_menu_gui() {
    gameInterface.inMenu = true;
}

void refresh_main_gui() {
    // Clear window
    wclear(gameInterface.gui.winMAIN);

    // Draw window
    if (gameInterface.inMenu == true) {
        menu_refresh_main_window();
    } else {
        refresh_level();
    }

    // Refresh window
    wrefresh(gameInterface.gui.winMAIN);
}

void refresh_right_menu_gui() {
    // Clear window
    wclear(gameInterface.gui.winTOOLS);

    // Draw window
    if (gameInterface.inMenu == true) {
        menu_refresh_right_menu();
    } else {
        refresh_player_menu();
    }

    // Refresh window
    wrefresh(gameInterface.gui.winTOOLS);
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

    // Check if the mouse is inside the main window
    if (posX >= 0 && posX < MATRICE_LEVEL_X && posY >= 0 && posY < MATRICE_LEVEL_Y) {
        if (gameInterface.inMenu == true) {
            menu_mouse_main_window(posX, posY);
        } else {
            game_mouse_level_window(posX, posY);
        }
    }

    // Check if the mouse is inside the right window
    else if (posX >= 62 && posX < 77 && posY >= 0 && posY < 20) {
        if (gameInterface.inMenu == true) {
            menu_mouse_right_menu(posX, posY);
        } else {
            game_mouse_player_menu(posX, posY);
        }
    }

    // Write down mouse position
    char text[100];
    sprintf(text, "Position : (Y, X) -> (%i, %i)", posY, posX);
    set_text_info_gui(text, 2, LBLUE_COLOR);
}

/**
 * @brief Function managing the keyboard events
 * 
 * @param key : character typed
 */
void control_handler_gui(int key) {
	int posX, posY;
    switch (key) {
        case KEY_MOUSE:
            // Mouse event handler
            if (mouse_getpos(&posX, &posY) == OK)
                mouse_event((short)posX, (short)posY);
        break;
        default: 
            if (gameInterface.inMenu == true) {
                menu_keyboard_handler(key);
            } else {
                game_keyboard_handler(key);
            }
        break;
    }
}

