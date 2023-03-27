
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#include "serveur_network.h"
#include "console_manager.h"
#include "utils.h"
#include "constants.h"

int pid_udp_processus;

void main_exit() {
    logs(L_INFO, "Network | Killing network processus");
    printf("Network | Killing network processus...\n");
    kill(pid_udp_processus, SIGINT);
    waitpid(pid_udp_processus, NULL, 0);
    closeLogs();
}

// Main method
int main(int argc, char *argv[]) {
    // create map folder if not exist
    char *path = "./maps/";

    // Check if the directory exists
    struct stat st;
    if (stat(path, &st) == -1) {
        // Directory does not exist, create it
        if (mkdir(path, 0700) == -1) {
            // Print an error message if the directory cannot be created
            printf("Cannot create directory: %s\n", path);
            exit(1);
        } else {
            // Print a message if the directory is created successfully
            printf("Directory created: %s\n", path);
        }
    } else if (S_ISDIR(st.st_mode)) {
        // Directory exists
        printf("Directory already exists: %s\n", path);
    } else {
        // Path exists but is not a directory
        printf("%s exists but is not a directory\n", path);
        exit(1);
    }

    // init network
    pid_udp_processus = init_network(argc, argv);
    if (pid_udp_processus == -1) {
        logs(L_INFO, "Network | Network init failed");
        printf("Network | Network init failed\n");
        return EXIT_FAILURE;
    }
    logs(L_INFO, "Network | Network init done PID : %d", pid_udp_processus);

    // Register exit function
    atexit(main_exit);

    // init console handler bloquant
    init_console();

    return EXIT_SUCCESS;
}

