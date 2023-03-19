
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
    printf("Network | Killing network processus...");
    kill(pid_udp_processus, SIGKILL);
}

// Main method
int main(int argc, char *argv[]) {
    // Register exit function
    atexit(main_exit);

    // init network
    pid_udp_processus = init_network(argc, argv);
    logs(L_INFO, "Network | Network init done PID : %d", pid_udp_processus);

    // init console handler bloquant
    init_console();

    return EXIT_SUCCESS;
}

