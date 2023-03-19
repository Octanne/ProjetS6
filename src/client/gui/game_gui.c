
#include "game_gui.h"

#include <locale.h>
#include <ncurses.h>

#include "utils.h"
#include "constants.h"
#include "gui_struct.h"
#include "level_update.h"

/**
 * @brief Function to clear the level and generate a new one
 */
void clear_level() {
	// Free level and generate a new one
    level_free(&(gameInterface.gameInfo.level));
    gameInterface.gameInfo.level = levelCreer();

	// Logs and refresh level
    logs(L_INFO, "Main | Level cleared");
    logs(L_INFO, "Main | Level value : %X", gameInterface.gameInfo.level);
    refresh_level();
}

/**
 * @brief Function to load a level and save the old one
 */
void load_level(int newLevel) {
    // TODO to change
    level_free(&(gameInterface.gameInfo.level));
    gameInterface.gameInfo.level = levelCreer();

    // Logs and refresh level
    logs(L_INFO, "Main | New level load : %d", newLevel);
    logs(L_INFO, "Main | Level %d : %d items loaded", newLevel, gameInterface.gameInfo.level.listeObjet.taille);
    set_text_info_gui("Level loaded", 1, GREEN_COLOR);

    refresh_level();
}

/**
 * @brief Refresh the level window from scratch.
 */
void refresh_level() {
	// Draw level
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
			// Get sprite data
            SpriteData spriteD = gameInterface.gameInfo.level.matriceSprite[y + x * MATRICE_LEVEL_Y];

			// Move cursor to sprite position
            wmove(gameInterface.gui.winMAIN, y, x);

			// Set color and draw sprite
            wattron(gameInterface.gui.winMAIN, COLOR_PAIR(spriteD.color));
            if (spriteD.specialChar)
				waddch(gameInterface.gui.winMAIN, spriteD.spSprite);
            else
				waddch(gameInterface.gui.winMAIN, spriteD.sprite);
            wattroff(gameInterface.gui.winMAIN, COLOR_PAIR(spriteD.color));
        }
    }
}

/**
 * @brief Refresh the tools menu from scratch.
 */
void refresh_player_menu() {
	// Draw player infos
    // Draw Keys
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 2, 2, "Keys");
    if (gameInterface.gameInfo.key1 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 2, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(PURPLE_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 2, ACS_LLCORNER);
    }
    if (gameInterface.gameInfo.key2 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(GREEN_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 4, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(GREEN_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 4, ACS_LLCORNER);
    }
    if (gameInterface.gameInfo.key3 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 6, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(YELLOW_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 6, ACS_LLCORNER);
    }
    if (gameInterface.gameInfo.key4 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(LBLUE_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 8, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(LBLUE_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 8, ACS_LLCORNER);
    }

    // Draw Lives
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 7, 2, "Lives");
    int i;
    for (i = 0; i < gameInterface.gameInfo.nbLives; i++) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(RED_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 9, 2 + 2*i, 'V');
    }

    // Draw Bombs
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 11, 2, "Bombs");
    for (i = 0; i < gameInterface.gameInfo.nbBombs; i++) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(RED_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 13, 2 + 2*i, 'O');
    }

    // Draw Level
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 15, 2, "Level");
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
    mvwprintw(gameInterface.gui.winTOOLS, 17, 2, " %03i ", gameInterface.gameInfo.currentLevel);

	// Refresh window
    wattroff(gameInterface.gui.winTOOLS, COLOR_PAIR(RED_COLOR));
}

/**
 * @brief Generate the game editor window
 */
void gen_game_window() {

    // Level window
    gameInterface.gui.cwinMAIN = newwin(22, 62, 0, 0);
    box(gameInterface.gui.cwinMAIN, 0, 0);
    wmove(gameInterface.gui.cwinMAIN, 0, 0);
    wprintw(gameInterface.gui.cwinMAIN, " Level ");
    wrefresh(gameInterface.gui.cwinMAIN);

	// Create a subwindow using derwin
    gameInterface.gui.winMAIN = derwin(gameInterface.gui.cwinMAIN, MATRICE_LEVEL_Y, MATRICE_LEVEL_X, 1, 1);
    wrefresh(gameInterface.gui.winMAIN);

    // Tools window
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

/**
 * @brief Generate the players menu
 */
void gen_player_menu() {
    gameInterface.gameInfo.nbLives = 5;
    gameInterface.gameInfo.nbBombs = 5;

    gameInterface.gameInfo.key1 = 1;
    gameInterface.gameInfo.key2 = 1;
    gameInterface.gameInfo.key3 = 1;
    gameInterface.gameInfo.key4 = 1;

    gameInterface.gameInfo.currentLevel = 1;

    refresh_player_menu();
}

void game_init_gui() {
    gen_game_window();
    logs(L_INFO, "Main | Game Window created!");
    gen_player_menu();
    logs(L_INFO, "Main | Tools menu created!");

    // Init level
    load_level(1);
}

void game_stop_gui() {
	// Free the level
    level_free(&(gameInterface.gameInfo.level));
}

void game_mouse_level_window(int x, int y) {
    // TODO if we want to use the mouse
}

void game_mouse_player_menu(int x, int y) {
    // TODO if we want to use the mouse
}

void game_keyboard_handler(int key) {
    switch (key) {
        case KEY_UP:
            // Write down the action
            set_text_info_gui("Action: UP", 1, GREEN_COLOR);
        break;
        
        case KEY_DOWN:
            // Write down the action
            set_text_info_gui("Action: DOWN", 1, GREEN_COLOR);
        break;

        case KEY_LEFT:
            // Write down the action
            set_text_info_gui("Action: LEFT", 1, GREEN_COLOR);
        break;

        case KEY_RIGHT:
            // Write down the action
            set_text_info_gui("Action: RIGHT", 1, GREEN_COLOR);
        break;

        case KEY_VALIDATE:
            // Write down the action
            set_text_info_gui("Action: VALIDATE", 1, GREEN_COLOR);
        break;

        default:
            // Write down the action
            set_text_info_gui("Action: UNKNOWN", 1, RED_COLOR);
        break;
    }
}

