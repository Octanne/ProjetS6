
#include "client_gui.h"

#include <locale.h>

#include "utils.h"
#include "constants.h"

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
    
    gen_game_window();
    logs(L_INFO, "Main | Game Window created!");
    gen_player_menu();
    logs(L_INFO, "Main | Tools menu created!");
}

/**
 * @brief Stop ncurses windows and free memory.
 */
void stop_gui() {
    delwin(gameInterface.gui.winLEVEL);
    delwin(gameInterface.gui.winTOOLS);
    delwin(gameInterface.gui.winINFOS);
    delwin(gameInterface.gui.cwinLEVEL);
    delwin(gameInterface.gui.cwinTOOLS);
    delwin(gameInterface.gui.cwinINFOS);

    ncurses_stop();
    logs(L_INFO, "Main | Ncurses windows deleted!");
}

/**
 * @brief Append text to the info window.
 */
void set_text_info(const char *text, int line, int color) {
	// Clear line
    mvwprintw(gameInterface.gui.winINFOS, line, 0, EMPTY_LINE);

	// Move cursor to line
    wmove(gameInterface.gui.winINFOS, line, 0);

	// Set color and print text
    wattron(gameInterface.gui.winINFOS, COLOR_PAIR(color));
    mvwprintw(gameInterface.gui.winINFOS, line, 0, "%s", text);
    wattroff(gameInterface.gui.winINFOS, COLOR_PAIR(color));

	// Refresh window
    wrefresh(gameInterface.gui.winINFOS);
}

/**
 * @brief Refresh the level window from scratch.
 */
void refresh_level(Level level) {
	// Clear window
    wclear(gameInterface.gui.winLEVEL);

	// Draw level
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
			// Get sprite data
            SpriteData spriteD = level.matriceSprite[y + x * MATRICE_LEVEL_Y];

			// Move cursor to sprite position
            wmove(gameInterface.gui.winLEVEL, y, x);

			// Set color and draw sprite
            wattron(gameInterface.gui.winLEVEL, COLOR_PAIR(spriteD.color));
            if (spriteD.specialChar)
				waddch(gameInterface.gui.winLEVEL, spriteD.spSprite);
            else
				waddch(gameInterface.gui.winLEVEL, spriteD.sprite);
            wattroff(gameInterface.gui.winLEVEL, COLOR_PAIR(spriteD.color));
        }
    }

	// Refresh window
    wrefresh(gameInterface.gui.winLEVEL);
}

/**
 * @brief Refresh the tools menu from scratch.
 */
void refresh_player_menu() {
	// Clear window
    wclear(gameInterface.gui.winTOOLS);

	// Draw player infos

    // Draw Keys
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 2, 2, "Keys");
    if (gameInterface.playerMenu.key1 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 2, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(PURPLE_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 2, ACS_LLCORNER);
    }
    if (gameInterface.playerMenu.key2 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(GREEN_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 4, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(GREEN_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 4, ACS_LLCORNER);
    }
    if (gameInterface.playerMenu.key3 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 6, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(YELLOW_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 6, ACS_LLCORNER);
    }
    if (gameInterface.playerMenu.key4 == 1) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(LBLUE_BLOCK));
        mvwaddch(gameInterface.gui.winTOOLS, 4, 8, ' ');
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(LBLUE_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 5, 8, ACS_LLCORNER);
    }


    // Draw Lives
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 7, 2, "Lives");
    int i;
    for (i = 0; i < gameInterface.playerMenu.nbLives; i++) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(RED_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 9, 2 + 2*i, 'V');
    }

    // Draw Bombs
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 11, 2, "Bombs");
    for (i = 0; i < gameInterface.playerMenu.nbBombs; i++) {
        wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(RED_COLOR));
        mvwaddch(gameInterface.gui.winTOOLS, 13, 2 + 2*i, 'O');
    }

    // Draw Level
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface.gui.winTOOLS, 15, 2, "Level");
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
    mvwprintw(gameInterface.gui.winTOOLS, 17, 2, " %03i ", gameInterface.playerMenu.currentLevel);

	// Refresh window
    wattroff(gameInterface.gui.winTOOLS, COLOR_PAIR(RED_COLOR));
    wrefresh(gameInterface.gui.winTOOLS);
}

/**
 * @brief Generate the game editor window
 */
void gen_game_window() {

    // Level window
    gameInterface.gui.cwinLEVEL = newwin(22, 62, 0, 0);
    box(gameInterface.gui.cwinLEVEL, 0, 0);
    wmove(gameInterface.gui.cwinLEVEL, 0, 0);
    wprintw(gameInterface.gui.cwinLEVEL, " Level ");
    wrefresh(gameInterface.gui.cwinLEVEL);

	// Create a subwindow using derwin
    gameInterface.gui.winLEVEL = derwin(gameInterface.gui.cwinLEVEL, MATRICE_LEVEL_Y, MATRICE_LEVEL_X, 1, 1);
    wrefresh(gameInterface.gui.winLEVEL);

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
    set_text_info("Press 'Q' to quit...", 0, RED_COLOR);
    wrefresh(gameInterface.gui.winINFOS);
}

/**
 * @brief Generate the players menu
 */
void gen_player_menu() {
    gameInterface.playerMenu.nbLives = 5;
    gameInterface.playerMenu.nbBombs = 5;

    gameInterface.playerMenu.key1 = 1;
    gameInterface.playerMenu.key2 = 1;
    gameInterface.playerMenu.key3 = 1;
    gameInterface.playerMenu.key4 = 1;

    gameInterface.playerMenu.currentLevel = 1;

    refresh_player_menu();
}

