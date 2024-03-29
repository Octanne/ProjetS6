
#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#include <pthread.h>

void ncurses_init();
void ncurses_stop();
void ncurses_colors();
void ncurses_init_mouse();
int mouse_getpos(short *x, short *y);
void palette();
void center_string(const char* str, const int dest_len, char* dest);

void initLogs();
void logs(int log_level, char *text_to_log, ...);
void closeLogs();

void lockMutexAvoidDeadLock(pthread_mutex_t *mutex1, pthread_mutex_t *mutex2);

#endif

