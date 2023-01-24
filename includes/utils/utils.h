#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#include "constants.h"
#include "st_benchmark.h"

/**
 * ncurses initialisation.
 */
void ncurses_init();

/**
 * Stop ncurses.
 */
void ncurses_stop();

/**
 * Colors initialisation.
 */
void ncurses_colors();

/**
 * Mouse initialisation.
 */
void ncurses_init_mouse();

/**
 * Get the mouse position.
 */
int mouse_getpos(int *x, int *y);

/**
 * initLogs() : initialize the logs file
*/
void initLogs();

/**
 * logs() : write a log in the logs file
*/
void logs(int log_level, char *text_to_log, ...);

/**
 * closeLogs() : close the logs file
*/
void closeLogs();

/**
 * palette() : initialize the colors palette
 */
void palette();

#endif