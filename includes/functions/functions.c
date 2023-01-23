#include "functions.h"

#include <ncurses.h>
#include <stdlib.h>

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

