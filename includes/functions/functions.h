#ifndef _FUNCTIONS_
#define _FUNCTIONS_

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



#endif