#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "includes/level/level.h"
#include "includes/utils/utils.h"
#include "includes/level_edit/level_edit.h"
#include "includes/system_save/system_save.h"
#include "includes/system_gui/system_gui.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

Level* level = NULL;
GameInterface* gameInterface = NULL;

void clear_level(Level* level) {
    level_free(level);
    level = NULL;
    level = levelCreer();
    refresh_level(level);
}

void load_level(int newLevel, int oldLevel) {
    file_t* file = load_file(FILENAME);

    // decalage des niveaux
    oldLevel--;
    newLevel--;

    if (save_level(file, oldLevel, level) == -1) logs(L_INFO, "Main | Error while saving level %d", oldLevel);
    if (get_level(file, newLevel, &level) == -1) {
        // Pas de niveau dans le fichier
        clear_level(level);
        logs(L_INFO, "Main | Level %d not found, empty level generated", newLevel);
    }

    // show table
    logs(L_INFO, "\n======================Affichage Tables======================\n\n%s======================Affichage Tables======================",show_table(file));

    free_file(file);

    logs(L_INFO, "Main | New level load : %d, Old level save : %d", newLevel, oldLevel);
    logs(L_INFO, "Main | Level %d : %d items loaded", newLevel, level->listeObjet->taille);
    refresh_level(level);

    set_text_info("Level loaded", 1, GREEN_COLOR);
}

void load_level_file() {
    logs(L_INFO, "Main | Loading first level from file...");
    file_t* file = load_file(FILENAME);
    logs(L_INFO, "Main | File loaded");

    if (get_level(file, 0, &level) == -1) {
        // Pas de niveau dans le fichier
        clear_level(level);
        logs(L_INFO, "Main | First level not found, empty level generated.");
    } else {
        logs(L_INFO, "Main | First level found and loaded.");
        logs(L_INFO, "Main | First level : %d items loaded", level->listeObjet->taille);
        refresh_level(level);
    }

    // show table
    logs(L_INFO, "\n======================Affichage Tables======================\n\n%s======================Affichage Tables======================",show_table(file));

    free_file(file);
}

void stop_game() {
    stop_gui();

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
            clear_level(level);
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
        refresh_level(level);
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

    // Init gui
    init_gui();

    // Load level
    load_level_file();

    // Launch control handler
    control_handler();

    return 0;
}