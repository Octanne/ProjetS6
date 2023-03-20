
#ifndef __GUI_UPDATER_H__
#define __GUI_UPDATER_H__

#include <ncurses.h>
#include <stdint.h>
#include <stdbool.h>

#include "level.h"
#include "net_message.h"

typedef struct {
    int8_t nbBombs;
    int8_t nbLives;

    int8_t key1;
    int8_t key2;
    int8_t key3;
    int8_t key4;

    int currentLevel;
    Level level;
} GameInfo;

typedef struct {
    char player_name[255];
} MenuInfo;

typedef struct {
    WINDOW* winMAIN;
    WINDOW* winTOOLS;
    WINDOW* winINFOS;

    WINDOW* cwinMAIN;
    WINDOW* cwinTOOLS;
    WINDOW* cwinINFOS;
} GUI;

typedef struct {
    GUI gui;
    union {
        GameInfo gameInfo;
        MenuInfo menuInfo;
    };
    bool inMenu;
} GameInterface;

extern GameInterface gameInterface;

void set_text_info_gui(const char *text, int line, int color);

int init_updater_gui();

void update_menu_gui(DataUpdateMenu *data);
void update_game_gui(DataUpdateGame *data);

#endif

