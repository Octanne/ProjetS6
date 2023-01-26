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

void init_gui(GameInterface *gameInterface);
void stop_gui(GameInterface *gameInterface);

void gen_game_editor_window(GameInterface *gameInterface);
void gen_tools_menu(GameInterface *gameInterface);

void refresh_level(GameInterface *gameInterface, Level *level);
void refresh_tools_menu(GameInterface *gameInterface);

void set_text_info(GameInterface *gameInterface, const char *text, int line, int color);

#endif