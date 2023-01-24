#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "includes/level/level.h"
#include "includes/utils/utils.h"
#include "includes/level_edit/level_edit.h"
#include "includes/table_addr/table_addr.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

typedef struct {
    int toolsSelected;
    int gateColorSelected;
    int doorNumberSelected;
    int levelNumberSelected;

    int inEdit;
} ToolsMenu;

WINDOW* winLEVEL;
WINDOW* winTOOLS;
WINDOW* winINFOS;

WINDOW* cwinLEVEL;
WINDOW* cwinTOOLS;
WINDOW* cwinINFOS;

ToolsMenu* toolsMenu;
Level* level = NULL;

void set_text_info(const char *text, int line, int color) {
    mvwprintw(winINFOS, line, 0, 
    "                                                                           ");
    wmove(winINFOS, line, 0);
    wattron(winINFOS, COLOR_PAIR(color));
    mvwprintw(winINFOS, line, 0, "%s", text);
    wattroff(winINFOS, COLOR_PAIR(color));
    wrefresh(winINFOS);
}

void gen_game_editor_window() {

    // Level window
    cwinLEVEL = newwin(22, 62, 0, 0);
    box(cwinLEVEL, 0, 0);
    wmove(cwinLEVEL, 0, 0);
    wprintw(cwinLEVEL, "Level");
    wrefresh(cwinLEVEL);

    winLEVEL = derwin(cwinLEVEL, 20, 60, 1, 1);
    wrefresh(winLEVEL);

    // Tools window
    cwinTOOLS = newwin(22, 15, 0, 62);
    box(cwinTOOLS, 0, 0);
    wmove(cwinTOOLS, 0, 0);
    wprintw(cwinTOOLS, "Tools");
    wrefresh(cwinTOOLS);

    winTOOLS = derwin(cwinTOOLS, 20, 13, 1, 1);
    wrefresh(winTOOLS);

    // Informations window
    cwinINFOS = newwin(5, 77, 22, 0);
    box(cwinINFOS, 0, 0);
    wmove(cwinINFOS, 0, 0);
    wprintw(cwinINFOS, "Informations");
    wrefresh(cwinINFOS);

    winINFOS = derwin(cwinINFOS, 3, 75, 1, 1);
    set_text_info("Press 'Q' to quit...", 0, RED_COLOR);
    wrefresh(winINFOS);
}

void refresh_level() {
    wclear(winLEVEL);
    int y, x;
    for (y = 0; y < 20; y++) {
        for (x = 0; x < 60; x++) {
            SpriteData* spriteD = level->matriceSprite[y+x*20];
            wattron(winLEVEL, COLOR_PAIR(spriteD->color));
            wmove(winLEVEL, y, x);
            if (spriteD->specialChar) waddch(winLEVEL, spriteD->spSprite);
            else waddch(winLEVEL, spriteD->sprite);
            wattroff(winLEVEL, COLOR_PAIR(spriteD->color));
        }
    }
    wrefresh(winLEVEL);
}

void gen_level_empty() {
    level_free(level);
    level = NULL;
    level = levelCreer();
    refresh_level();
}

void load_level(int newLevel, int oldLevel) {
    // TODO: Load level from file or empty level if not found
    file_t* file = load_file(FILENAME);

    if (save_level(file, oldLevel, level) == -1) logs(L_INFO, "Main | Error while saving level %d", oldLevel);
    if (get_level(file, newLevel, &level) == -1) {
        // Pas de niveau dans le fichier
        gen_level_empty();
        logs(L_INFO, "Main | Level %d not found, empty level generated", newLevel);
    }

    free_file(file);

    logs(L_INFO, "Main | New level load : %d, Old level save : %d", newLevel, oldLevel);
    logs(L_INFO, "Main | Level %d : %d items loaded", newLevel, level->listeObjet->taille);
    refresh_level();

    set_text_info("Level loaded", 1, GREEN_COLOR);
}

void load_level_file() {
    logs(L_INFO, "Main | Loading first level from file...");
    file_t* file = load_file(FILENAME);
    logs(L_INFO, "Main | File loaded");

    // show table
    logs(L_INFO, "\n======================Affichage Tables======================\n\n%s======================Affichage Tables======================",show_table(file));

    if (get_level(file, 1, &level) == -1) {
        // Pas de niveau dans le fichier
        gen_level_empty();
        logs(L_INFO, "Main | First level not found, empty level generated.");
    } else {
        logs(L_INFO, "Main | First level found and loaded.");
        logs(L_INFO, "Main | First level : %d items loaded", level->listeObjet->taille);
        refresh_level();
    }

    free_file(file);
}

void stop_game() {
    delwin(winLEVEL);
    delwin(winTOOLS);
    delwin(winINFOS);
    delwin(cwinLEVEL);
    delwin(cwinTOOLS);
    delwin(cwinINFOS);
    ncurses_stop();
    logs(L_INFO, "Main | Ncurses windows deleted, now loading file...");

    file_t* file = load_file(FILENAME);
    logs(L_INFO, "Main | Saving level %d...", toolsMenu->levelNumberSelected);
    save_level(file, toolsMenu->levelNumberSelected, level);
    logs(L_INFO, "Main | Level %d saved", toolsMenu->levelNumberSelected);
    // show table
    logs(L_INFO, "\n======================Affichage Tables======================\n\n%s======================Affichage Tables======================",show_table(file));
    free_file(file);

    free(toolsMenu);
    level_free(level);
    closeLogs();
}

void refresh_tools_menu() {
    wclear(winTOOLS);

    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 0 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 0, 2, "Delete");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 1 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 1, 2, "Block");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 2 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 2, 2, "Ladder");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 3 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 3, 2, "Trap");
    
    // Gate
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 4 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 4, 2, "Gate");
    wattron(winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
    mvwprintw(winTOOLS, 4, 8, " ");
    wattron(winTOOLS, COLOR_PAIR(GREEN_BLOCK));
    mvwprintw(winTOOLS, 4, 9, " ");
    wattron(winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
    mvwprintw(winTOOLS, 4, 10, " ");
    wattron(winTOOLS, COLOR_PAIR(DBLUE_BLOCK));
    mvwprintw(winTOOLS, 4, 11, " ");
    wattron(winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(winTOOLS, 5, toolsMenu->gateColorSelected+8, "^");

    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 5 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 5, 2, "Key");

    // Door
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 6 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 6, 2, "Door");
    wattron(winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(winTOOLS, 6, 8, "<%02i>", toolsMenu->doorNumberSelected);

    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 7 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 7, 2, "Exit");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 8 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 8, 2, "Start");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 9 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 9, 2, "Robot");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 10 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 10, 2, "Probe");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 11 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 11, 2, "Life");
    wattron(winTOOLS, COLOR_PAIR(toolsMenu->toolsSelected == 12 ? WHITE_COLOR : RED_COLOR));
    mvwprintw(winTOOLS, 12, 2, "Bomb");

    wattron(winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(winTOOLS, 14, 0, "Current level");
    wattron(winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(winTOOLS, 16, 5, "%03i", toolsMenu->levelNumberSelected);
    wattron(winTOOLS, COLOR_PAIR(ARROW_BUTTON));
    mvwprintw(winTOOLS, 16, 3, "<");
    mvwprintw(winTOOLS, 16, 9, ">");

    // Sel cursor
    wattron(winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(winTOOLS, toolsMenu->toolsSelected, 0, ">");

    wattron(winTOOLS, COLOR_PAIR(RED_BUTTON));
    mvwprintw(winTOOLS, 18, 3, "DELETE");

    wattroff(winTOOLS, COLOR_PAIR(RED_COLOR));
    wrefresh(winTOOLS);
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

void mouse_toolsWindow(int posX, int posY) {
    if (posX >= 62 && posX < 77 && posY >= 0 && posY < 20) {
        // increase level number
        if (posY == 16 && posX == 71) {
            if (toolsMenu->levelNumberSelected < 999) {
                toolsMenu->levelNumberSelected++;
                char text[100];
                sprintf(text, "Chargement du niveau '%03i'.", toolsMenu->levelNumberSelected);
                load_level(toolsMenu->levelNumberSelected, toolsMenu->levelNumberSelected - 1);
                set_text_info(text, 1, WHITE_COLOR);
                refresh_tools_menu();
            }
        }
        // decrease level number
        else if (posY == 16 && posX == 65) {
            if (toolsMenu->levelNumberSelected > 1) {
                toolsMenu->levelNumberSelected--;
                char text[100];
                sprintf(text, "Chargement du niveau '%03i'.", toolsMenu->levelNumberSelected);
                set_text_info(text, 1, WHITE_COLOR);
                load_level(toolsMenu->levelNumberSelected, toolsMenu->levelNumberSelected + 1);
                refresh_tools_menu();
            }
        }
        // delete level
        else if (posY == 18 && posX >= 65 && posX <= 70) {
            logs(L_INFO, "Main | Remise à zéro du niveau %d", toolsMenu->levelNumberSelected);
            gen_level_empty();
            set_text_info("Le niveau est remis a zero.", 1, WHITE_COLOR);
        }
    }
}

void mouse_levelWindow(int posX, int posY) {
    if (posX >= 0 && posX < 60 && posY >= 0 && posY < 20) {
        // check de l'outils selectionnée
        int success = 0;
        switch(toolsMenu->toolsSelected) {
            case 0:
                // Delete
                success = supprimerObjet(level, posX, posY);
                break;
            case 1:
                // Block
                success = poserBlock(level, posX, posY);
                break;
            case 2:
                // Ladder
                success = poserLadder(level, posX, posY);
                break;
            case 3:
                // Trap
                success = poserTrap(level, posX, posY);
                break;
            case 4:
                // Gate
                success = poserGate(level, posX, posY, toolsMenu->gateColorSelected);
                break;
            case 5:
                // Key
                success = poserKey(level, posX, posY, toolsMenu->gateColorSelected);
                break;
            case 6:
                // Door
                success = poserDoor(level, posX, posY, toolsMenu->doorNumberSelected);
                break;
            case 7:
                // Exit
                success = poserExit(level, posX, posY);
                break;
            case 8:
                // Start
                success = poserStart(level, posX, posY);
                break;
            case 9:
                // Robot
                success = poserRobot(level, posX, posY);
                break;
            case 10:
                // Probe
                success = poserProbe(level, posX, posY);
                break;
            case 11:
                // Life
                success = poserVie(level, posX, posY);
                break;
            case 12:
                // Bomb
                success = poserBomb(level, posX, posY);
                break;
        }
        refresh_level();
        if (success == 1) {
            set_text_info("Action d'edition effectuee avec succes.", 1, GREEN_COLOR);
        } else set_text_info("Vous ne pouvez pas effectuer cette action ici.", 1, YELLOW_COLOR);
    }
}

void mouse_event(int posX, int posY) {
    // convert to window level coordinates
    posX -= 1;
    posY -= 1;
    // check if mouse is in window level
    mouse_levelWindow(posX, posY);
    // check if mouse is in window tools
    mouse_toolsWindow(posX, posY);

    // Write down mouse position
    char text[100];
    sprintf(text, "Position : (Y,X) -> (%i,%i)", posY,posX);
    set_text_info(text, 2, LBLUE_COLOR);
}

void control_handler() {
    int ch;
    while((ch = getch()) != KEY_QUIT_GAME) {
        switch(ch) {
            case KEY_UP:
                set_text_info("Action: UP", 1, GREEN_COLOR);
                if (toolsMenu->inEdit) {
                    if (toolsMenu->toolsSelected > 0) {
                        toolsMenu->toolsSelected--;
                    }
                    refresh_tools_menu();
                }
                break;
            case KEY_DOWN:
                set_text_info("Action: DOWN", 1, GREEN_COLOR);
                if (toolsMenu->inEdit) {
                    if (toolsMenu->toolsSelected < 12) {
                        toolsMenu->toolsSelected++;
                    }
                    refresh_tools_menu();
                }
                break;
            case KEY_LEFT:
                set_text_info("Action: LEFT", 1, GREEN_COLOR);
                if (toolsMenu->inEdit) {
                    if (toolsMenu->toolsSelected == 4) {
                        if (toolsMenu->gateColorSelected > 0) {
                            toolsMenu->gateColorSelected--;
                        }
                    }
                    if (toolsMenu->toolsSelected == 6) {
                        if (toolsMenu->doorNumberSelected > 1) {
                            toolsMenu->doorNumberSelected--;
                        }
                    }
                    refresh_tools_menu();
                }
                break;
            case KEY_RIGHT:
                set_text_info("Action: RIGHT", 1, GREEN_COLOR);
                if (toolsMenu->inEdit) {
                    if (toolsMenu->toolsSelected == 4) {
                        if (toolsMenu->gateColorSelected < 3) {
                            toolsMenu->gateColorSelected++;
                        }
                    }
                    if (toolsMenu->toolsSelected == 6) {
                        if (toolsMenu->doorNumberSelected < 99) {
                            toolsMenu->doorNumberSelected++;
                        }
                    }
                    refresh_tools_menu();
                }
                break;
            case KEY_VALIDATE:
                set_text_info("Action: VALIDATE", 1, GREEN_COLOR);
                break;
            case KEY_MOUSE:
                int posX, posY;
                if (mouse_getpos(&posX,&posY) == OK) {
                    mouse_event(posX, posY);
                }
                break;
        }
    }
}

void main_exit() {
    stop_game();
}

int main(void)
{
    // Register exit function
    atexit(main_exit);

    // Init ncurses
    setlocale(LC_ALL, "");
    ncurses_init();
    ncurses_init_mouse();
    ncurses_colors();
    palette();
    getmaxyx(stdscr, LINES, COLS);

    // Init game windows
    gen_game_editor_window();
    gen_tools_menu();
    load_level_file();

    control_handler();

    return 0;
}