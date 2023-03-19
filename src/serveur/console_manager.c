
#include "console_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "utils.h"
#include "constants.h"

void init_console() {
    bool isRun = true;
    while(isRun) {
        char command[2048];
        if (scanf("%2048s", command) == -1) {
            isRun = false;
            break;
        }
        isRun = command_handler(command);
    }
}

bool command_handler(char *command) {
    if (strcmp(command, "exit") == 0) {
        return false;
    } else if (strcmp(command, "help") == 0) {
        printf("Available commands:\n");
        printf("exit: exit the program\n");
        printf("help: display this message\n");
        return true;
    } else {
        printf("Unknown command: %s\n", command);
        return true;
    }
}