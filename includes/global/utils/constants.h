
#ifndef CONSTANTS_H
#define CONSTANTS_H

// #####################
// #    LOGS SYSTEM    #

#define L_INFO 1
#define L_DEBUG 2
#define lvl_log(log_level) (log_level == L_INFO ? "INFO" : (log_level == L_DEBUG ? "DEBUG" : "UNKNOWN"))
#define LOGS_FILE "logs/latest.log"
#define LOGS_FOLDER "logs/"
#define LOGS_FOLDER_ARCHIVES "logs/archives/"
#define LOGS_ACTIVE 2 // 2 = debug, 1 = info, 0 = inactive
#define LOGS_MAX_LENGTH 3500

#define FILENAME "level.dat"
#define EMPTY_LINE "                                                                           "

// #####################
// #      KEYBOARD     #

#define KEY_QUIT_GAME 113
#define KEY_VALIDATE 10

// #####################
// #      COLORS       #

#define WHITE_COLOR 1
#define RED_COLOR 2
#define GREEN_COLOR 3
#define LBLUE_COLOR 4
#define DBLUE_COLOR 5
#define YELLOW_COLOR 6
#define PURPLE_COLOR 7

#define LBLUE_BLOCK 8
#define PURPLE_BLOCK 9
#define GREEN_BLOCK 10
#define YELLOW_BLOCK 11
#define DBLUE_BLOCK 12
#define RED_BLOCK 13
#define GRAY_BLOCK 14

#define RED_BUTTON 15
#define ARROW_BUTTON 16

// #####################
// #      OBJECTS      #

#define DELETE_ID 0
#define BLOCK_ID 1
#define LADDER_ID 2
#define TRAP_ID 3
#define GATE_ID 4
#define KEY_ID 5
#define DOOR_ID 6
#define EXIT_ID 7
#define START_ID 8
#define ROBOT_ID 9
#define PROBE_ID 10
#define HEART_ID 11
#define BOMB_ID 12
#define PLAYER_ID 13

#define TOTAL_TOOLS 12
#define RIGHT_ORIENTATION 1
#define LEFT_ORIENTATION 0


// #####################
// #    Liste Type     #

#define TYPE_PLAYER 0
#define TYPE_OBJET 1
#define TYPE_NET_MESSAGE 2
#define TYPE_PARTIE_INFO 3
#define TYPE_MAP_INFO 4
#define TYPE_SOCKADDR_IN 5

// #####################
// #       Level       #

#define MATRICE_LEVEL_X 60
#define MATRICE_LEVEL_Y 20
#define MATRICE_LEVEL_SIZE 1200

#define MAX_PLAYERS 4

#endif

