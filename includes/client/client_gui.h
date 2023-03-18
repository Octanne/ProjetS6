
#ifndef CLIENT_GUI_H
#define CLIENT_GUI_H

#include <ncurses.h>

#include "level.h"
#include "utils.h"

typedef struct {
    int8_t nbBombs;
    int8_t nbLives;

    int8_t key1;
    int8_t key2;
    int8_t key3;
    int8_t key4;

    int currentLevel;
} PlayerMenu;

typedef struct {
    WINDOW* winLEVEL;
    WINDOW* winTOOLS;
    WINDOW* winINFOS;

    WINDOW* cwinLEVEL;
    WINDOW* cwinTOOLS;
    WINDOW* cwinINFOS;
} GUI;

typedef struct {
    GUI gui;
    PlayerMenu playerMenu;
} GameInterface;

extern GameInterface gameInterface;

void init_gui();
void stop_gui();

void gen_game_window();
void gen_player_menu();

void refresh_level(Level level);
void refresh_player_menu();

void set_text_info(const char *text, int line, int color);

#endif

