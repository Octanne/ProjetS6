
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "serveur_network.h"
#include "utils.h"
#include "constants.h"

// Main method
int main(int argc, char *argv[]) {

    // init network
    int pid_udp_processus = init_network(argc, argv);
    logs(L_INFO, "Network | Network init done PID : %d", pid_udp_processus);

    // WAIT FOR UDP PROCESSUS
    int status;
    waitpid(pid_udp_processus, &status, 0);

    return EXIT_SUCCESS;
}

