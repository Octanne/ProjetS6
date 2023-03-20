
#include "gui_updater.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

#include "client_gui.h"

#include "control_handler.h"

int pid_gui_updater;
int msqid_gui_updater;
bool gui_updater_running = true;

GameInterface *gameI_global;
pthread_mutex_t gameI_global_lock = PTHREAD_MUTEX_INITIALIZER;

void close_updater_gui_trigger() {
    logs(L_INFO, "GUI Updater | Closing gui_updater asked...");
    gui_updater_running = false;
}

void close_updater_gui() {
    gui_updater_running = false;
    // Close the queue
    if (msgctl(msqid_gui_updater, IPC_RMID, NULL) < 0) {
        logs(L_INFO, "GUI Updater  | Error deleting message queue : %s", strerror(errno));
        perror("Error deleting message queue");
        exit(EXIT_FAILURE);
    }
    // Lock the mutex and check return value
    if (pthread_mutex_lock(&gameI_global_lock) != 0) {
        logs(L_INFO, "GUI Updater | Error while locking the mutex");
        exit(EXIT_FAILURE);
    }
    // Free the game interface
    stop_gui(gameI_global);
    // Unlock the mutex and check return value
    if (pthread_mutex_unlock(&gameI_global_lock) != 0) {
        logs(L_INFO, "GUI Updater | Error while unlocking the mutex");
        exit(EXIT_FAILURE);
    }
    logs(L_INFO, "GUI Updater | GUI Updater closed");
}

int init_updater_gui() {
    logs(L_INFO, "GUI Updater | Init gui_updater...");

    // Create the queue
    //key_t key = ftok("/tmp/gui_s6", 'b');
    if ((msqid_gui_updater = msgget(1036, S_IRUSR | S_IWUSR | IPC_CREAT)) == -1) {
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
        // Init graphics
        GameInterface gameI;
        init_gui(&gameI);

        // Init global variable
        gameI_global = &gameI;

        // Signal handler via sigaction
        struct sigaction sa;
        sa.sa_handler = close_updater_gui_trigger;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("sigaction");
            logs(L_INFO, "Network | Error setting signal handler");
            exit(EXIT_FAILURE);
        }

        // Pthread for the control handler
        pthread_t thread_control_handler;
        if (pthread_create(&thread_control_handler, NULL, control_handler, NULL) != 0) {
            logs(L_INFO, "GUI Updater | Error while creating the thread");
            exit(EXIT_FAILURE);
        }

        while (gui_updater_running) {
            // block until a message is available
            DataChangeMessage data;
            if (msgrcv(msqid_gui_updater, &data, sizeof(DataChangeMessage), 0, 0) == -1) {
                // Si SIGINT
                if (errno == EINTR) {
                    logs(L_INFO, "GUI Updater | gui_updater closed while waiting for an update of gui");
                    close_updater_gui();
                    exit(EXIT_SUCCESS);
                }
                logs(L_DEBUG, "GUI Updater | Error while receiving a message");
                exit(EXIT_FAILURE);
            }

            // Lock the mutex
            if (pthread_mutex_lock(&gameI_global_lock) != 0) {
                logs(L_INFO, "GUI Updater | Error while locking the mutex");
                exit(EXIT_FAILURE);
            }

            // TODO

            // Unlock the mutex
            if (pthread_mutex_unlock(&gameI_global_lock) != 0) {
                logs(L_INFO, "GUI Updater | Error while unlocking the mutex");
                exit(EXIT_FAILURE);
            }
        }
        
        close_updater_gui();

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

void write_text_info_bar(DataTextInfoGUI *data) {
    // TODO
}