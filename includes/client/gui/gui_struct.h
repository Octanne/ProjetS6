
#ifndef __GUI_STRUCT_H__
#define __GUI_STRUCT_H__

#include <ncurses.h>
#include <stdbool.h>

#include "level.h"

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

typedef struct gui {
    WINDOW* winMAIN;
    WINDOW* winTOOLS;
    WINDOW* winINFOS;

    WINDOW* cwinMAIN;
    WINDOW* cwinTOOLS;
    WINDOW* cwinINFOS;
} GUI;

typedef struct {
    union {
        GameInfo gameInfo;
        MenuInfo menuInfo;
    };
    bool inMenu;
    
    GUI gui;
} GameInterface;

#endif