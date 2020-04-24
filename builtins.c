#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"
#include "debug.h"
#include "proclist.h"

void cd(struct cmdline *cmd) {
    DEBUG_PRINT("Executing built-in command 'cd'\n");
    char *newDir = cmd->seq[0][1];
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
    exit(EXIT_SUCCESS);
}

void list(proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'list'\n");
    if (*procList == NULL) { // Empty list
        printf("\n");
        return;
    }

    // Get the last two processes
    int lastID, previousID;
    getLastTwoProcesses(procList, &lastID, &previousID);

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
        else if (current->state == ACTIVE) {
            printf("Running\t\t      ");
        }
        else {
            printf("Done\t\t      ");
        }
        // Print the command executed by the process
        printf("%s\n", current->commandName);

        // Go to the next process
        current = current->next;
    }
}

int cmdlineToPID(struct cmdline *cmd, proc_t *procList) {
    int pid = 0;
    if (cmd->seq[0][1] != NULL) {
        int id = atoi(cmd->seq[0][1]);
        int pid = getPID(procList, id);
    }
    return pid;
}

void stop(struct cmdline *cmd, proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'stop'\n");

    int pid = cmdlineToPID(cmd, procList);
    if (pid == 0) { // No process found
        return;
    }

    kill(pid, SIGTSTP);
    DEBUG_PRINTF("[%d] Process stopped\n", pid);
}

void bg(struct cmdline *cmd, proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'bg'\n");

    int pid = cmdlineToPID(cmd, procList);
    if (pid == 0) { // No process found
        return;
    }

    kill(pid, SIGCONT);
    DEBUG_PRINTF("[%d] Process resumed\n", pid);
}

void fg(struct cmdline *cmd, proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'fg'\n");

    int pid = cmdlineToPID(cmd, procList);
    if (pid == 0) { // No process found
        return;
    }

    kill(pid, SIGCONT);
    DEBUG_PRINTF("[%d] Process resumed\n", pid);

    printf("TODO: TAKE PROCESS TO FOREGROUND\n");
}