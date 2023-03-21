
#ifndef __GUI_STRUCT_H__
#define __GUI_STRUCT_H__

#include <ncurses.h>
#include <stdbool.h>
#include <pthread.h>

#include "level.h"
#include "player.h"

typedef struct {
    Player player;
    Level *level;
} GameInfo;

typedef struct {
    char name[36];
    int nbPlayers;
    int maxPlayers;
    int status;

    bool set;
    WINDOW* winTAB;
} TabPartie;

typedef struct {
    char player_name[255];

    int numPage;
    int selPartie;
    int nbParties;
    TabPartie tabPartie[4];
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
    pthread_mutex_t mutex;
} GameInterface;

#endif