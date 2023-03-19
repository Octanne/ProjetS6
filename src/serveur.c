
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
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
    // init network
    pid_udp_processus = init_network(argc, argv);
    logs(L_INFO, "Network | Network init done PID : %d", pid_udp_processus);

    // Register exit function
    atexit(main_exit);

    // init console handler bloquant
    init_console();

    return EXIT_SUCCESS;
}

