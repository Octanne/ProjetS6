
#include "gui_struct.h"

#include "constants.h"

/**
 * @brief Append text to the info window.
 */
void set_text_info_gui(const char *text, int line, int color) {
	// Clear line
    mvwprintw(gameInterface.gui.winINFOS, line, 0, EMPTY_LINE);

	// Move cursor to line
    wmove(gameInterface.gui.winINFOS, line, 0);

	// Set color and print text
    wattron(gameInterface.gui.winINFOS, COLOR_PAIR(color));
    mvwprintw(gameInterface.gui.winINFOS, line, 0, "%s", text);
    wattroff(gameInterface.gui.winINFOS, COLOR_PAIR(color));

	// Refresh window
    wrefresh(gameInterface.gui.winINFOS);
}

