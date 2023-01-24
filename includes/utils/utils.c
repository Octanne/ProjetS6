#include "utils.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>

/**
 * ncurses initialisation.
 */
void ncurses_init() {
  initscr();	        // Start ncurses mode
  cbreak();	            // Disable the buffer cache
  noecho();             // Disable the display of typed characters
  keypad(stdscr, TRUE);	// Activate specific keys (arrows)
  refresh();            // Update display
  curs_set(FALSE);      // Hide the cursor
}

/**
 * Stop ncurses.
 */
void ncurses_stop() {
  endwin();
}

/**
 * Colors initialisation.
 */
void ncurses_colors() {
  // Check color support
  if(has_colors() == FALSE) {
    ncurses_stop();
    fprintf(stderr, "The terminal doesn't support colors.\n");
    exit(EXIT_FAILURE);
  }

  // Activate colors
  start_color();
}

/**
 * Mouse initialisation.
 */
void ncurses_init_mouse() {
  if(!mousemask(BUTTON1_PRESSED, NULL)) {
    ncurses_stop();
    fprintf(stderr, "Mouse isn't supported.\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Get the mouse position.
 */
int mouse_getpos(int *x, int *y) {
  MEVENT event;
  int result = getmouse(&event);
  if(result == OK) {
    *x = event.x;
    *y = event.y;
  }
  return result;
}

/**
 * palette() : initialize the colors palette
 */
void palette() {
    init_pair(WHITE_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(RED_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(LBLUE_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(DBLUE_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(YELLOW_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PURPLE_COLOR, COLOR_MAGENTA, COLOR_BLACK);

    init_pair(LBLUE_BLOCK, COLOR_BLACK, COLOR_CYAN);
    init_pair(DBLUE_BLOCK, COLOR_BLACK, COLOR_BLUE);
    init_pair(PURPLE_BLOCK, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(GREEN_BLOCK, COLOR_BLACK, COLOR_GREEN);
    init_pair(YELLOW_BLOCK, COLOR_BLACK, COLOR_YELLOW);
    init_pair(RED_BLOCK, COLOR_BLACK, COLOR_RED);

    init_pair(RED_BUTTON, COLOR_WHITE, COLOR_RED);
    init_pair(ARROW_BUTTON, COLOR_BLACK, COLOR_WHITE);
}

// Global variables
int file_logs_desc = -1;

/**
 * initLogs() : initialize the logs file
*/
void initLogs() {
    // Create the logs folder if it doesn't exist
    if (mkdir(LOGS_FOLDER, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        // show erno
        if (errno != EEXIST) {
            fprintf(stderr, "Error: can't create the logs folder.\n");
            exit(EXIT_FAILURE);
        }
    }
    // Open the file with open
    file_logs_desc = open(LOGS_FILE, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    // check if the file is open
    if (file_logs_desc == -1) {
        fprintf(stderr, "Error: can't open the logs file.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * logs() : write a log in the logs file
*/
void logs(char *text_to_log, ...) {
    if (file_logs_desc == -1) initLogs();

    // Format time in char * with the format: [HH:MM:SS]
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time[12];
    strftime(time, sizeof(time), "%H:%M:%S", t);

    // format text_to_log en y ajoutant les arguments non nommés de la fonction
    char text_form[LOGS_MAX_LENGTH];

    va_list args;
    va_start(args, text_to_log);
    vsprintf(text_form, text_to_log, args);

    va_end(args);

    // Open the file in append mode check the success
    char final_text[LOGS_MAX_LENGTH+15];
    sprintf(final_text, "[%s] %s\n", time, text_form);
    if (write(file_logs_desc, final_text, strlen(final_text)+1) == -1) {
        fprintf(stderr, "Error while writing in the logs file.\n");
    }
}

/**
 * closeLogs() : close the logs file
*/
void closeLogs() {
    // Write the end of the log file
    if(file_logs_desc == -1) {
        initLogs();
        logs("No logs has been written during this session.");
    }
    
    logs("End of the session of the game.");
    if(close(file_logs_desc) == -1) {
        fprintf(stderr, "Error while closing the logs file.\n");
    }

    // Move the file to the logs folder
    // Format time in char * with the format: HH-MM-SS_DD-MM-YYYY
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time[30];
    strftime(time, sizeof(time), "%d-%m-%Y_%H-%M-%S", t);
    
    char file_name[30] = LOGS_FOLDER;
    strcat(file_name, time);
    strcat(file_name, ".log");
    if(rename(LOGS_FILE, file_name) == -1) {
        fprintf(stderr, "Error while moving the logs file.\n");
    }

    // Reset the file_logs_desc
    file_logs_desc = -1;
}