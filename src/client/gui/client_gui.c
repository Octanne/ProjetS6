
#include "client_gui.h"

#include <locale.h>

#include "utils.h"
#include "constants.h"

#include "menu_gui.h"
#include "game_gui.h"

/**
 * @brief Init ncurses windows.
 */
void init_gui(GameInterface *gameI) {
    logs(L_INFO, "Main | Ncurses windows init...");
    setlocale(LC_ALL, "");
    ncurses_init();
    ncurses_init_mouse();
    ncurses_colors();
    palette();
    getmaxyx(stdscr, LINES, COLS);
    logs(L_INFO, "Main | Ncurses windows init success!");

    gen_right_menu_gui(gameI);
    gen_main_gui(gameI);

    if (gameI->inMenu == true) {
        menu_init_gui(gameI);
    } else {
        game_init_gui(gameI);
    }
}

void switch_gui(GameInterface *gameI) {
    if (gameI->inMenu == true) {
        gameI->inMenu = false;
        menu_stop_gui(gameI);
        game_init_gui(gameI);
        set_text_info_gui(gameI, "Vous venez de rejoindre la partie", 1, GREEN_COLOR);
    } else {
        gameI->inMenu = true;
        game_stop_gui(gameI);
        menu_init_gui(gameI);
    }
}

/**
 * @brief Stop ncurses windows and free memory.
 */
void stop_gui(GameInterface *gameI) {
    delwin(gameI->gui.winMAIN);
    delwin(gameI->gui.winTOOLS);
    delwin(gameI->gui.winINFOS);
    delwin(gameI->gui.cwinMAIN);
    delwin(gameI->gui.cwinTOOLS);
    delwin(gameI->gui.cwinINFOS);

    ncurses_stop();
    logs(L_INFO, "Main | Ncurses windows deleted!");

    if (gameI->inMenu == true) {
        menu_stop_gui(gameI);
    } else {
        game_stop_gui(gameI);
    }
}

void gen_main_gui(GameInterface *gameI) {
    // Main window
    gameI->gui.cwinMAIN = newwin(22, 62, 0, 0);
    box(gameI->gui.cwinMAIN, 0, 0);
    wrefresh(gameI->gui.cwinMAIN);

	// Create a subwindow using derwin
    gameI->gui.winMAIN = derwin(gameI->gui.cwinMAIN, MATRICE_LEVEL_Y, MATRICE_LEVEL_X, 1, 1);
    wrefresh(gameI->gui.winMAIN);

    // Right window
    gameI->gui.cwinTOOLS = newwin(22, 15, 0, 62);
    box(gameI->gui.cwinTOOLS, 0, 0);
    wmove(gameI->gui.cwinTOOLS, 0, 0);
    //wprintw(gameI->gui.cwinTOOLS, " Tools ");
    wrefresh(gameI->gui.cwinTOOLS);

	// Create a subwindow using derwin
    gameI->gui.winTOOLS = derwin(gameI->gui.cwinTOOLS, 20, 13, 1, 1);
    wrefresh(gameI->gui.winTOOLS);

    // Informations window
    gameI->gui.cwinINFOS = newwin(5, 77, 22, 0);
    box(gameI->gui.cwinINFOS, 0, 0);
    wmove(gameI->gui.cwinINFOS, 0, 0);
    wprintw(gameI->gui.cwinINFOS, " Informations ");
    wrefresh(gameI->gui.cwinINFOS);

	// Create a subwindow using derwin
    gameI->gui.winINFOS = derwin(gameI->gui.cwinINFOS, 3, 75, 1, 1);

	// Set text info and refresh window
    set_text_info_gui(gameI, "Press 'Q' to quit...", 0, RED_COLOR);
    wrefresh(gameI->gui.winINFOS);
}

void gen_right_menu_gui(GameInterface *gameI) {
    gameI->inMenu = true;
}

void refresh_main_gui(GameInterface *gameI) {
    // Clear window
    wclear(gameI->gui.winMAIN);

    // Draw window
    if (gameI->inMenu == true) {
        menu_refresh_main_window(gameI);
    } else {
        refresh_level(gameI);
    }

    // Refresh window
    wrefresh(gameI->gui.winMAIN);
}

void refresh_right_menu_gui(GameInterface *gameI) {
    // Clear window
    wclear(gameI->gui.winTOOLS);

    // Draw window
    if (gameI->inMenu == true) {
        menu_refresh_right_menu(gameI);
    } else {
        refresh_player_menu(gameI);
    }

    // Refresh window
    wrefresh(gameI->gui.winTOOLS);
}

/**
 * @brief Append text to the info window.
 */
void set_text_info_gui(GameInterface *gameI, const char *text, int line, int color) {
	// Clear line
    mvwprintw(gameI->gui.winINFOS, line, 0, EMPTY_LINE);

	// Move cursor to line
    wmove(gameI->gui.winINFOS, line, 0);

	// Set color and print text
    wattron(gameI->gui.winINFOS, COLOR_PAIR(color));
    mvwprintw(gameI->gui.winINFOS, line, 0, "%s", text);
    wattroff(gameI->gui.winINFOS, COLOR_PAIR(color));

	// Refresh window
    wrefresh(gameI->gui.winINFOS);
}