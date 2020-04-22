#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"
#include "debug.h"

void changeDirectory(char *newDir) {
    DEBUG_PRINT("Executing built-in command 'cd'\n");
    if (newDir == NULL) { // No arguments given to cd
        char *HOME = getenv("HOME");
        DEBUG_PRINT("cd: Changing current directory to HOME\n");
        setenv("PWD", HOME, true);
        chdir(HOME);
    }
    else {
        DEBUG_PRINTF("cd: Trying to change current directory to %s\n", newDir);
        if (chdir(newDir) != -1) {
            setenv("PWD", newDir, true);
        }
        else {
            printf("minishell: cd: %s: No such file or directory\n", newDir);
        }
    }
}

void exitShell() {
    DEBUG_PRINT("exit: exiting shell ...\n");
    exit(0);
}