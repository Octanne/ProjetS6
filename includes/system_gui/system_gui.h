#ifndef SYSTEM_GUI_H
#define SYSTEM_GUI_H

#include <ncurses.h>

#include "../level/level.h"
#include "../utils/utils.h"

typedef struct {
    int toolsSelected;
    int gateColorSelected;
    int doorNumberSelected;
    int levelNumberSelected;

    int inEdit;
} ToolsMenu;

typedef struct {
    WINDOW* winLEVEL;
    WINDOW* winTOOLS;
    WINDOW* winINFOS;

    WINDOW* cwinLEVEL;
    WINDOW* cwinTOOLS;
    WINDOW* cwinINFOS;
} GUI;

typedef struct {
    GUI* gui;
    ToolsMenu* toolsMenu;
} GameInterface;

GameInterface *gameInterface;

void init_gui();
void stop_gui();

void gen_game_editor_window();
void gen_tools_menu();

void refresh_level(Level *level);
void refresh_tools_menu();

void set_text_info(const char *text, int line, int color);

#endif