
#include "gui_updater.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "utils.h"
#include "constants.h"


#define MESSAGE_QUEUE_KEY "ProjetS6GUIUpdater"

int pid_gui_updater;
int msqid_gui_updater;
bool gui_updater_running = true;

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

void close_updater_gui() {
    logs(L_INFO, "GUI Updater | Closing gui_updater...");
    gui_updater_running = false;

    // Close the queue
    if (msgctl(msqid_gui_updater, IPC_RMID, NULL) == -1) {
        logs(L_DEBUG, "GUI Updater | Error while closing the queue");
        exit(EXIT_FAILURE);
    }
}

int init_updater_gui() {
    logs(L_INFO, "GUI Updater | Init gui_updater...");

    // Create the queue
    key_t key = ftok(MESSAGE_QUEUE_KEY, 1);
    if ((msqid_gui_updater = msgget(key, 0666 | IPC_CREAT)) == -1) {
        logs(L_DEBUG, "GUI Updater | Error while creating the queue");
        exit(EXIT_FAILURE);
    }

    // Create the process
    pid_gui_updater = fork();
    if (pid_gui_updater == -1) {
        logs(L_DEBUG, "GUI Updater | Error while creating the process");
        exit(EXIT_FAILURE);
    }

    // Child process
    if (pid_gui_updater == 0) {
        while (gui_updater_running) {
            // block until a message is available
            DataUpdateGame data;
            if (msgrcv(msqid_gui_updater, &data, sizeof(DataUpdateGame), 0, 0) == -1) {
                logs(L_DEBUG, "GUI Updater | Error while receiving a message");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }

    return pid_gui_updater;
}

void update_game_gui(DataUpdateGame *data) {
    // TODO
}

void update_menu_gui(DataUpdateMenu *data) {
    // TODO
}

