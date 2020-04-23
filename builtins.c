#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"
#include "debug.h"
#include "proclist.h"

void cd(char *newDir) {
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

void exitShell(proc_t *procList) {
    DEBUG_PRINT("exit: exiting shell ...\n");
    deleteProcList(procList);
    exit(0);
}

void list(proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'list'\n");
    if (*procList == NULL) { // Empty list
        printf("\n");
        return;
    }

    // Get the two last processes
    int lastID, previousID;
    getTwoLastProcesses(procList, &lastID, &previousID);

    // Loop trough each process in the list and display its information
    proc_t current = *procList;
    while (current != NULL) {

        // Print the ID of the process
        printf("[%d]", current->id);
        // Print a special character if the process is the last or second-to-last modified
        if (current->id == lastID) {
            printf("+  ");
        }
        else if (current->id == previousID) {
            printf("-  ");
        }
        else {
            printf("   ");
        }
        // Print the state of the process
        if (current->state == SUSPENDED) {
            printf("Stopped\t\t      ");
        }
        else {
            printf("Running\t\t      ");
        }
        // Print the command executed by the process
        printf("%s\n", current->commandName);

        // Go to the next process
        current = current->next;
    }
    printf("\n");
}
