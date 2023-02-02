#include "system_gui.h"

#include <locale.h>

void init_gui() {
    setlocale(LC_ALL, "");
    ncurses_init();
    ncurses_init_mouse();
    ncurses_colors();
    palette();
    getmaxyx(stdscr, LINES, COLS);

    gameInterface = malloc(sizeof(GameInterface));
    gen_game_editor_window();
    gen_tools_menu();
}

void stop_gui() {
    delwin(gameInterface->gui->winLEVEL);
    delwin(gameInterface->gui->winTOOLS);
    delwin(gameInterface->gui->winINFOS);
    delwin(gameInterface->gui->cwinLEVEL);
    delwin(gameInterface->gui->cwinTOOLS);
    delwin(gameInterface->gui->cwinINFOS);

    free(gameInterface->gui);
    free(gameInterface->toolsMenu);
    free(gameInterface);
    ncurses_stop();
    logs(L_INFO, "Main | Ncurses windows deleted!");
}

void set_text_info(const char *text, int line, int color) {
    mvwprintw(gameInterface->gui->winINFOS, line, 0, 
    "                                                                           ");
    wmove(gameInterface->gui->winINFOS, line, 0);
    wattron(gameInterface->gui->winINFOS, COLOR_PAIR(color));
    mvwprintw(gameInterface->gui->winINFOS, line, 0, "%s", text);
    wattroff(gameInterface->gui->winINFOS, COLOR_PAIR(color));
    wrefresh(gameInterface->gui->winINFOS);
}

void refresh_level(Level *level) {
    wclear(gameInterface->gui->winLEVEL);
    int y, x;
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            SpriteData* spriteD = level->matriceSprite[y+x*20];
            wattron(gameInterface->gui->winLEVEL, COLOR_PAIR(spriteD->color));
            wmove(gameInterface->gui->winLEVEL, y, x);
            if (spriteD->specialChar) waddch(gameInterface->gui->winLEVEL, spriteD->spSprite);
            else waddch(gameInterface->gui->winLEVEL, spriteD->sprite);
            wattroff(gameInterface->gui->winLEVEL, COLOR_PAIR(spriteD->color));
        }
    }
    wrefresh(gameInterface->gui->winLEVEL);
}

void refresh_tools_menu() {
    wclear(gameInterface->gui->winTOOLS);

    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 0 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 0, 2, "Delete");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 1 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 1, 2, "Block");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 2 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 2, 2, "Ladder");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 3 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 3, 2, "Trap");
    
    // Gate
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 4 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 4, 2, "Gate");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
    mvwprintw(gameInterface->gui->winTOOLS, 4, 8, " ");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(GREEN_BLOCK));
    mvwprintw(gameInterface->gui->winTOOLS, 4, 9, " ");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
    mvwprintw(gameInterface->gui->winTOOLS, 4, 10, " ");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(DBLUE_BLOCK));
    mvwprintw(gameInterface->gui->winTOOLS, 4, 11, " ");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 5, gameInterface->toolsMenu->gateColorSelected+8, "^");

    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 5 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 5, 2, "Key");

    // Door
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 6 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 6, 2, "Door");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 6, 8, "<%02i>", gameInterface->toolsMenu->doorNumberSelected);

    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 7 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 7, 2, "Exit");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 8 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 8, 2, "Start");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 9 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 9, 2, "Robot");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 10 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 10, 2, "Probe");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 11 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 11, 2, "Life");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(gameInterface->toolsMenu->toolsSelected == 12 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 12, 2, "Bomb");

    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 14, 0, "Current level");
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, 16, 5, "%03i", gameInterface->toolsMenu->levelNumberSelected);
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(ARROW_BUTTON));
    mvwprintw(gameInterface->gui->winTOOLS, 16, 3, "<");
    mvwprintw(gameInterface->gui->winTOOLS, 16, 9, ">");

    // Sel cursor
    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameInterface->gui->winTOOLS, gameInterface->toolsMenu->toolsSelected, 0, ">");

    wattron(gameInterface->gui->winTOOLS, COLOR_PAIR(RED_BUTTON));
    mvwprintw(gameInterface->gui->winTOOLS, 18, 3, "DELETE");

    wattroff(gameInterface->gui->winTOOLS, COLOR_PAIR(RED_COLOR));
    wrefresh(gameInterface->gui->winTOOLS);
}

void gen_game_editor_window() {

    // Level window
    gameInterface->gui->cwinLEVEL = newwin(22, 62, 0, 0);
    box(gameInterface->gui->cwinLEVEL, 0, 0);
    wmove(gameInterface->gui->cwinLEVEL, 0, 0);
    wprintw(gameInterface->gui->cwinLEVEL, "Level");
    wrefresh(gameInterface->gui->cwinLEVEL);

    gameInterface->gui->winLEVEL = derwin(gameInterface->gui->cwinLEVEL, 20, 60, 1, 1);
    wrefresh(gameInterface->gui->winLEVEL);

    // Tools window
    gameInterface->gui->cwinTOOLS = newwin(22, 15, 0, 62);
    box(gameInterface->gui->cwinTOOLS, 0, 0);
    wmove(gameInterface->gui->cwinTOOLS, 0, 0);
    wprintw(gameInterface->gui->cwinTOOLS, "Tools");
    wrefresh(gameInterface->gui->cwinTOOLS);

    gameInterface->gui->winTOOLS = derwin(gameInterface->gui->cwinTOOLS, 20, 13, 1, 1);
    wrefresh(gameInterface->gui->winTOOLS);

    // Informations window
    gameInterface->gui->cwinINFOS = newwin(5, 77, 22, 0);
    box(gameInterface->gui->cwinINFOS, 0, 0);
    wmove(gameInterface->gui->cwinINFOS, 0, 0);
    wprintw(gameInterface->gui->cwinINFOS, "Informations");
    wrefresh(gameInterface->gui->cwinINFOS);

    gameInterface->gui->winINFOS = derwin(gameInterface->gui->cwinINFOS, 3, 75, 1, 1);
    set_text_info("Press 'Q' to quit...", 0, RED_COLOR);
    wrefresh(gameInterface->gui->winINFOS);
}

void gen_tools_menu() {
    gameInterface->toolsMenu = malloc(sizeof(ToolsMenu));
    gameInterface->toolsMenu->toolsSelected = 0;
    gameInterface->toolsMenu->gateColorSelected = 0;
    gameInterface->toolsMenu->doorNumberSelected = 1;
    gameInterface->toolsMenu->levelNumberSelected = 1;

    // Focus fleche on tools menu
    gameInterface->toolsMenu->inEdit = 1;
    refresh_tools_menu();
}
