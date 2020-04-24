/*
 * A minimalist command line interface between the user and the operating system
 */

#define _GNU_SOURCE /* WCONTINUED */

// Minishell prompt
#define PS1 "\033[0;33m%s\033[0;0m@\033[0;34mminishell\033[0m:\033[0;32m[%s]\033[0m$ "

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "debug.h"
#include "proclist.h"
#include "readcmd.h"

// Global variables
proc_t *procList;

/*
 * Function: execExternalCommand
 * -----------------------------
 *   Execute an external command, a subprocess will be forked
 *
 *   cmd: the command to execute
 */
void execExternalCommand(struct cmdline *cmd, proc_t *procList) {
    int pidFork, wstatus;

    fflush(stdout);   // Flush stdout to give an empty buffer to the child process
    pidFork = fork(); // Make a child process to execute the command

    if (pidFork < 0) {
        perror("fork");
        exit(1);
    }
    else if (pidFork == 0) { // Child process

        DEBUG_PRINTF("[%d] Child process executing command '%s'\n", getpid(), cmd->seq[0][0]);
        execvp(cmd->seq[0][0], cmd->seq[0]);
        printf("Unknown command\n"); // If execvp returns, the command failed
        exit(1);
    }
    else { // Parent process
        if (cmd->backgrounded) {
            int newID = addProcess(procList, pidFork, cmd->seq[0]);
            printf("[%d] %d\n", newID, pidFork);
            DEBUG_PRINTF("[%d] Parent process started a child process in the background\n",
                         getpid());
        }
        else {
            DEBUG_PRINTF("[%d] Parent process waiting for the end of its child process\n",
                         getpid());
            // Wait for the child to display the command's results
            waitpid(pidFork, &wstatus, 0);
        }
    }
}

/*
 * Function: treatCommand
 * ----------------------
 *   Treat a given command
 *
 *   cmd: the command to treat
 */
void treatCommand(struct cmdline *cmd, proc_t *procList) {
    char *cmdName = cmd->seq[0][0];
    if (!strcmp(cmdName, "cd")) {
        cd(cmd);
    }
    else if (!strcmp(cmdName, "exit")) {
        exitShell(procList);
    }
    else if (!strcmp(cmdName, "list") || !strcmp(cmdName, "jobs")) {
        list(procList);
    }
    else if (!strcmp(cmdName, "stop")) {
        stop(cmd, procList);
    }
    else if (!strcmp(cmdName, "bg")) {
        bg(cmd, procList);
    }
    else if (!strcmp(cmdName, "fg")) {
        fg(cmd, procList);
    }
    else {
        execExternalCommand(cmd, procList);
    }
}

/*
 * Function: childHandler
 * ----------------------
 *   Handle SIGCHLD
 */
void childHandler() {
    int childState, childPID;
    do {
        childPID = (int)waitpid(-1, &childState, WNOHANG | WUNTRACED | WCONTINUED);
        if ((childPID == -1) && (errno != ECHILD)) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        else if (childPID > 0) {
            if (WIFSTOPPED(childState)) {
                DEBUG_PRINTF("[%d] Child stopped by signal %d\n", childPID, WSTOPSIG(childState));
                setProcessStatusByPID(procList, childPID, SUSPENDED);
            }
            else if (WIFCONTINUED(childState)) {
                DEBUG_PRINTF("[%d] Child resumed\n", childPID);
                setProcessStatusByPID(procList, childPID, ACTIVE);
            }
            else if (WIFEXITED(childState)) {
                DEBUG_PRINTF("[%d] Child exited, status=%d\n", childPID, WEXITSTATUS(childState));
                setProcessStatusByPID(procList, childPID, DONE);
            }
            else if (WIFSIGNALED(childState)) {
                DEBUG_PRINTF("[%d] Child killed by signal %d\n", childPID, WTERMSIG(childState));
                removeProcessByPID(procList, childPID);
            }
        }
    } while (childPID > 0);
}

int main() {
    // Associate signals to their handlers
    signal(SIGCHLD, childHandler);

    // Create the process list
    procList = initProcList();

    struct cmdline *cmd;
    // Main loop
    while (true) {
        // Show the prompt
        printf(PS1, getenv("USER"), getenv("PWD"));
        fflush(stdout);

        // Read a command from standard input and execute it
        cmd = readcmd();
        // Print terminated processes and delete them from the list
        updateProcList(procList);
        if (cmd == NULL) { // Exit if CTRL+D is pressed to avoid infinite loop
            DEBUG_PRINT("CTRL+D entered\n");
            exitShell(procList);
        }
        else if (cmd->seq == NULL || *(cmd->seq) == NULL) { // Handle empty line
            DEBUG_PRINT("Empty line entered\n");
        }
        else {
            DEBUG_PRINTF("Treating command '%s'\n", cmd->seq[0][0]);
            treatCommand(cmd, procList);
        }
    }
}