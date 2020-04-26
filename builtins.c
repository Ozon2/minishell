#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
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
    printProcList(procList);
}

int cmdlineToPID(struct cmdline *cmd, proc_t *procList) {
    int lastID, previousID, id;
    getLastTwoProcesses(procList, &lastID, &previousID);
    int pid = getPID(procList, lastID); // If no arguments, return last modified process
    if (cmd->seq[0][1] != NULL) {
        if (*(cmd->seq[0][1]) == '+')
            id = lastID;
        else if (*(cmd->seq[0][1]) == '-')
            id = previousID;
        else
            id = atoi(cmd->seq[0][1]);
        pid = getPID(procList, id);
    }
    return pid;
}

void stop(struct cmdline *cmd, proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'stop'\n");

    int pid = cmdlineToPID(cmd, procList);
    if (pid == 0) { // No process found
        printf("minishell: stop: no such job\n");
        return;
    }

    kill(pid, SIGTSTP);
    setProcessStatusByPID(procList, pid, SUSPENDED);
    printProcessByPID(procList, pid);
    DEBUG_PRINTF("[%d] Process stopped\n", pid);
}

void bg(struct cmdline *cmd, proc_t *procList) {
    DEBUG_PRINT("Executing built-in command 'bg'\n");

    int pid = cmdlineToPID(cmd, procList);
    if (pid == 0) { // No process found
        printf("minishell: bg: no such job\n");
        return;
    }

    kill(pid, SIGCONT);
    DEBUG_PRINTF("[%d] Process resumed\n", pid);
}

void fg(struct cmdline *cmd, proc_t *procList, int *foregroundPID, bool *stopReceived) {
    DEBUG_PRINT("Executing built-in command 'fg'\n");

    int pid = cmdlineToPID(cmd, procList);
    if (pid == 0) { // No process found
        printf("minishell: fg: no such job\n");
        return;
    }

    kill(pid, SIGCONT);
    DEBUG_PRINTF("[%d] Process resumed\n", pid);

    *foregroundPID = pid;

    // Wait for the child to finish or to be stopped
    int wpid;
    do {
        wpid = waitpid(pid, NULL, WNOHANG | WUNTRACED);
    } while (wpid == 0 && !(*stopReceived));

    DEBUG_PRINTF("[%d] Process finished or stopped\n", pid);
}