
#ifndef __CONSOLE_MANAGER_H__
#define __CONSOLE_MANAGER_H__

#include <stdbool.h>

void init_console();
bool command_handler(char *command); // return true if handler should continue

#endif

