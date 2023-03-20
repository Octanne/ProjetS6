
#ifndef _FUNCTIONS_
#define _FUNCTIONS_

void ncurses_init();
void ncurses_stop();
void ncurses_colors();
void ncurses_init_mouse();
int mouse_getpos(int *x, int *y);
void palette();
void center_string(const char* str, const int dest_len, char* dest);

void initLogs();
void logs(int log_level, char *text_to_log, ...);
void closeLogs();

#endif

