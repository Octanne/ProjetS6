
#ifndef __GUI_STRUCT_H__
#define __GUI_STRUCT_H__

#include <ncurses.h>
#include <stdbool.h>
#include <pthread.h>

#include "level.h"
#include "player.h"
#include "net_struct.h"

typedef struct {
    Player player;
    Level *level;
} GameInfo;

typedef struct {
    PartieInfo info;
    WINDOW* winTAB;
} TabPartie;

typedef struct {
    MapInfo info;
    WINDOW* winTAB;
} TabMap;

typedef struct {
    int numPage;
    int selPartie;
    int nbParties;
    
    TabPartie tabPartie[4];
} TabPartieMenu;

typedef struct {
    int numPage;
    int selMap;
    int nbMaps;

    int maxPlayers;
    TabMap tabMap[6];
} CreatePartieMenu;

typedef struct {
    char player_name[255];
    bool inCreateMenu;
    
    bool waitToJoin;
    int selPartieID;

    CreatePartieMenu createPartieMenu;
    TabPartieMenu tabPartieMenu;
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
    NetworkSocket *netSocket;
    pthread_mutex_t mutex;
} GameInterface;

#endif