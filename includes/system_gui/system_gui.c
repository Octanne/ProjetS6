#include "system_gui.h"

#include <locale.h>

void init_gui() {
    setlocale(LC_ALL, "");
    ncurses_init();
    ncurses_init_mouse();
    ncurses_colors();
    palette();
    getmaxyx(stdscr, LINES, COLS);

    gen_game_editor_window();
    gen_tools_menu();
}

void stop_gui() {
    delwin(gui->winLEVEL);
    delwin(gui->winTOOLS);
    delwin(gui->winINFOS);
    delwin(gui->cwinLEVEL);
    delwin(gui->cwinTOOLS);
    delwin(gui->cwinINFOS);
    ncurses_stop();
    logs(L_INFO, "Main | Ncurses windows deleted!");
}

void set_text_info(const char *text, int line, int color) {
    mvwprintw(gui->winINFOS, line, 0, 
    "                                                                           ");
    wmove(gui->winINFOS, line, 0);
    wattron(gui->winINFOS, COLOR_PAIR(color));
    mvwprintw(gui->winINFOS, line, 0, "%s", text);
    wattroff(gui->winINFOS, COLOR_PAIR(color));
    wrefresh(gui->winINFOS);
}

void refresh_level(Level *level) {
    wclear(gui->winLEVEL);
    int y, x;
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            SpriteData* spriteD = level->matriceSprite[y+x*20];
            wattron(gui->winLEVEL, COLOR_PAIR(spriteD->color));
            wmove(gui->winLEVEL, y, x);
            if (spriteD->specialChar) waddch(gui->winLEVEL, spriteD->spSprite);
            else waddch(gui->winLEVEL, spriteD->sprite);
            wattroff(gui->winLEVEL, COLOR_PAIR(spriteD->color));
        }
    }
    wrefresh(gui->winLEVEL);
}

void refresh_tools_menu() {
    wclear(gui->winTOOLS);

    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 0 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 0, 2, "Delete");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 1 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 1, 2, "Block");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 2 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 2, 2, "Ladder");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 3 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 3, 2, "Trap");
    
    // Gate
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 4 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 4, 2, "Gate");
    wattron(gui->winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
    mvwprintw(gui->winTOOLS, 4, 8, " ");
    wattron(gui->winTOOLS, COLOR_PAIR(GREEN_BLOCK));
    mvwprintw(gui->winTOOLS, 4, 9, " ");
    wattron(gui->winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
    mvwprintw(gui->winTOOLS, 4, 10, " ");
    wattron(gui->winTOOLS, COLOR_PAIR(DBLUE_BLOCK));
    mvwprintw(gui->winTOOLS, 4, 11, " ");
    wattron(gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gui->winTOOLS, 5, toolsMenu->gateColorSelected+8, "^");

    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 5 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 5, 2, "Key");

    // Door
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 6 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 6, 2, "Door");
    wattron(gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gui->winTOOLS, 6, 8, "<%02i>", toolsMenu->doorNumberSelected);

    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 7 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 7, 2, "Exit");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 8 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 8, 2, "Start");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 9 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 9, 2, "Robot");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 10 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 10, 2, "Probe");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 11 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 11, 2, "Life");
    wattron(gui->winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 12 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(gui->winTOOLS, 12, 2, "Bomb");

    wattron(gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gui->winTOOLS, 14, 0, "Current level");
    wattron(gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gui->winTOOLS, 16, 5, "%03i", toolsMenu->levelNumberSelected);
    wattron(gui->winTOOLS, COLOR_PAIR(ARROW_BUTTON));
    mvwprintw(gui->winTOOLS, 16, 3, "<");
    mvwprintw(gui->winTOOLS, 16, 9, ">");

    // Sel cursor
    wattron(gui->winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gui->winTOOLS, toolsMenu->toolsSelected, 0, ">");

    wattron(gui->winTOOLS, COLOR_PAIR(RED_BUTTON));
    mvwprintw(gui->winTOOLS, 18, 3, "DELETE");

    wattroff(gui->winTOOLS, COLOR_PAIR(RED_COLOR));
    wrefresh(gui->winTOOLS);
}

void gen_game_editor_window() {

    // Level window
    gui->cwinLEVEL = newwin(22, 62, 0, 0);
    box(gui->cwinLEVEL, 0, 0);
    wmove(gui->cwinLEVEL, 0, 0);
    wprintw(gui->cwinLEVEL, "Level");
    wrefresh(gui->cwinLEVEL);

    gui->winLEVEL = derwin(gui->cwinLEVEL, 20, 60, 1, 1);
    wrefresh(gui->winLEVEL);

    // Tools window
    gui->cwinTOOLS = newwin(22, 15, 0, 62);
    box(gui->cwinTOOLS, 0, 0);
    wmove(gui->cwinTOOLS, 0, 0);
    wprintw(gui->cwinTOOLS, "Tools");
    wrefresh(gui->cwinTOOLS);

    gui->winTOOLS = derwin(gui->cwinTOOLS, 20, 13, 1, 1);
    wrefresh(gui->winTOOLS);

    // Informations window
    gui->cwinINFOS = newwin(5, 77, 22, 0);
    box(gui->cwinINFOS, 0, 0);
    wmove(gui->cwinINFOS, 0, 0);
    wprintw(gui->cwinINFOS, "Informations");
    wrefresh(gui->cwinINFOS);

    gui->winINFOS = derwin(gui->cwinINFOS, 3, 75, 1, 1);
    set_text_info("Press 'Q' to quit...", 0, RED_COLOR);
    wrefresh(gui->winINFOS);
}

void gen_tools_menu() {
    toolsMenu = malloc(sizeof(ToolsMenu));
    toolsMenu->toolsSelected = 0;
    toolsMenu->gateColorSelected = 0;
    toolsMenu->doorNumberSelected = 1;
    toolsMenu->levelNumberSelected = 1;

    // Focus fleche on tools menu
    toolsMenu->inEdit = 1;
    refresh_tools_menu();
}
