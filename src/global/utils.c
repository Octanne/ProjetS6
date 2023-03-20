
#include "utils.h"
#include "constants.h"

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
#include <sys/stat.h>

/**
 * @brief ncurses initialisation.
 */
void ncurses_init() {
	initscr();				// Start ncurses mode
	cbreak();				// Disable the buffer cache
	noecho();				// Disable the display of typed characters
	keypad(stdscr, TRUE);	// Activate specific keys (arrows)
	refresh();				// Update display
	curs_set(FALSE);		// Hide the cursor
}

/**
 * @brief Stop ncurses.
 */
void ncurses_stop() {
	endwin();
}

/**
 * @brief Colors initialisation.
 */
void ncurses_colors() {
	// Check color support
	if (has_colors() == FALSE) {
		ncurses_stop();
		perror("The terminal doesn't support colors.\n");
		exit(EXIT_FAILURE);
	}

	// Activate colors
	start_color();
}

/**
 * @brief Mouse initialisation.
 */
void ncurses_init_mouse() {
	if (!mousemask(BUTTON1_PRESSED, NULL)) {
		ncurses_stop();
		perror("Mouse isn't supported.\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Get the mouse position.
 * 
 * @param x : mouse position on X axis
 * @param y : mouse position on Y axis
 * 
 * @return error code (OK or ERR)
 */
int mouse_getpos(int *x, int *y) {
	MEVENT event;
	int result = getmouse(&event);
	if (result == OK) {
		*x = event.x;
		*y = event.y;
	}
	return result;
}

/**
 * @brief Initialize the colors palette
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
 * @brief Initialize the logs file
 */
void initLogs() {

	// Create the logs folder if it doesn't exist
	if (mkdir(LOGS_FOLDER, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		// show erno
		if (errno != EEXIST) {
			perror("Error: can't create the logs folder.\n");
			exit(EXIT_FAILURE);
		}
	}

	// Open the file with open
	file_logs_desc = open(LOGS_FILE, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
	// check if the file is open
	if (file_logs_desc == -1) {
		perror("Error: can't open the logs file.\n");
		exit(EXIT_FAILURE);
	}

	// Write the logs file header
	if (write(file_logs_desc, "\n", 1) == -1) {
		perror("Error while writing file header in the logs file.\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Write a log in the logs file
 * 
 * @param log_level : log level (2 = debug, 1 = info, 0 = inactive)
 * @param text_to_log : text to log
 * @param ... : arguments to add to the text_to_log
 */
void logs(int log_level, char *text_to_log, ...) {
	// Check log level
	if (log_level > LOGS_ACTIVE) return;

	// Initialize the logs file if it's not already done
	if (file_logs_desc == -1)
		initLogs();

	// Format time in char * with the format: [HH:MM:SS]
	char time_string[12];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(time_string, sizeof(time_string), "%H:%M:%S", t);

	// Format text_to_log en y ajoutant les arguments non-nommés de la fonction
	char text_form[LOGS_MAX_LENGTH];

	// Get the arguments
	va_list args;
	va_start(args, text_to_log);
	vsprintf(text_form, text_to_log, args);
	va_end(args);

	// Format the final text
	char final_text[LOGS_MAX_LENGTH + 26];
	sprintf(final_text, "[%s][%s] %s\n", time_string, lvl_log(log_level), text_form);

	// Open the file in append mode check the success
	if (write(file_logs_desc, final_text, strlen(final_text)) == -1) {
		perror("Error while writing in the logs file.\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Close the logs file
 */
void closeLogs() {
	// Write the end of the log file
	if (file_logs_desc == -1) {
		initLogs();
		logs(L_INFO, "No logs has been written during this session.");
	}
	
	logs(L_INFO, "End of the session of the game.\n");

	// Create the logs folder archives if it doesn't exist
	if (mkdir(LOGS_FOLDER_ARCHIVES, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		// show erno
		if (errno != EEXIST) {
			perror("Error: can't create the logs folder.\n");
			exit(EXIT_FAILURE);
		}
	}

	// Move the file to the logs folder
	// Format time in char * with the format: [YYYY-MM-DD_HH-MM-SS]
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	char time_string[30];
	strftime(time_string, sizeof(time_string), "%Y-%m-%d_%H-%M-%S", t);

	// Format the file name
	char file_name[30] = LOGS_FOLDER_ARCHIVES;
	strcat(file_name, time_string);
	strcat(file_name, ".log");

	// Open the destination file for writing
	int dest_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (dest_fd == -1) {
		perror("Error while opening the archived logs file.\n");
		exit(EXIT_FAILURE);
	}

	// Allocate a buffer for reading the file
	char buffer[4096];

	// Lseek to the beginning of the file
	if (lseek(file_logs_desc, 0, SEEK_SET) == -1) {
		perror("Error while lseek to the beginning of the file.\n");
		exit(EXIT_FAILURE);
	}

	// Read from the logs file and write to the archived dests file
	int bytes_read;
	while ((bytes_read = read(file_logs_desc, buffer, sizeof(buffer))) > 0) {
		if (write(dest_fd, buffer, bytes_read) == -1) {
			perror("Error while writing in the archived logs file.\n");
			exit(EXIT_FAILURE);
		}
	}

	// Close the source and destination files
	if (close(file_logs_desc) == -1 || close(dest_fd) == -1) {
		perror("Error while closing the logs file.\n");
		exit(EXIT_FAILURE);
	}

	// Reset the file_logs_desc
	file_logs_desc = -1;
}

void center_string(const char* str, const int dest_len, char* dest) {
    int str_len = strlen(str);
    int spaces = dest_len - str_len;
    int left_spaces = spaces / 2;
    int right_spaces = spaces - left_spaces;
    sprintf(dest, "%*s%s%*s", left_spaces, " ", str, right_spaces, " ");
}

