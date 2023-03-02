
#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#include "constants.h"
#include "st_benchmark.h"

void ncurses_init();
void ncurses_stop();
void ncurses_colors();
void ncurses_init_mouse();
int mouse_getpos(int *x, int *y);
void palette();

void initLogs();
void logs(int log_level, char *text_to_log, ...);
void closeLogs();

#endif

