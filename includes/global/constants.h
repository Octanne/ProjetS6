
#ifndef CONSTANTS_H
#define CONSTANTS_H

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

#define KEY_QUIT_GAME 113
#define KEY_VALIDATE 10

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

#define RED_BUTTON 14
#define ARROW_BUTTON 15

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

#endif
