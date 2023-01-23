#include "constants.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

// Global variables
FILE *file_logs;

void initLogs() {
    // Open the file in append mode
    file_logs = fopen("logs.txt", "a");
}

void logs(char *text_to_log, ...) {
    if (file_logs == NULL) initLogs();

    // Format time in char * with the format: [HH:MM:SS]
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time[12];
    strftime(time, sizeof(time), "%H:%M:%S", t);

    // format text_to_log en y ajoutant les arguments non nommés de la fonction
    char text_form[1500];

    va_list args;
    va_start(args, text_to_log);
    vsprintf(text_form, text_to_log, args);

    va_end(args);

    // Open the file in append mode
    fprintf(file_logs, "[%s] %s\n", time, text_form);

    // Flush the file
    fflush(file_logs);
}

void closeLogs() {
    // Write the end of the log file
    if(file_logs == NULL) {
        initLogs();
        logs("No logs has been written during this session.");
    }
    
    logs("End of the session of the game.");
    fclose(file_logs);
    file_logs = NULL;
}