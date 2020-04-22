/*
 * A minimalist command line interface between the user and the operating system
 */

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

// Minishell prompt
#define PS1 "\033[0;33m%s\033[0;0m@\033[0;34mminishell\033[0m:\033[0;32m[%s]\033[0m$ "

struct cmdline *cmd;

/*
 * Function: execExternalCommand
 * -----------------------------
 *   Execute an external command, a subprocess will be forked
 *
 *   cmd: the command to execute
 */
void execExternalCommand(struct cmdline *cmd) {
    int pidFork, wstatus;

    fflush(stdout);   // Flush stdout to give an empty buffer to the child process
    pidFork = fork(); // Make a child process to execute the command

    if (pidFork < 0) {

        perror("fork");
        exit(1);
    }
    else if (pidFork == 0) { // Child process

        DEBUG_PRINTF("[%d] Executing command '%s'\n", getpid(), cmd->seq[0][0]);
        execvp(cmd->seq[0][0], cmd->seq[0]);
        printf("Unknown command\n"); // If execvp returns, it has failed
        exit(1);
    }
    else { // Parent process
        if (!cmd->backgrounded) {
            DEBUG_PRINTF("[%d] Parent process waiting for the end of its child process\n",
                         getpid());
            // Wait for the child to display the command's results
            waitpid(pidFork, &wstatus, 0);
            if WIFEXITED (wstatus) { // Child ended with exit
                DEBUG_PRINTF("[%d] Child process ended with exit %d\n", getpid(),
                             WEXITSTATUS(wstatus));
            }
            else if WIFSIGNALED (wstatus) { // Child ended by a signal
                DEBUG_PRINTF("[%d] Child process killed by signal %d\n", getpid(),
                             WTERMSIG(wstatus));
            }
        }
        else {
            printf("[1] %d\n", pidFork);
            DEBUG_PRINTF("\n[%d] Parent process started a child process in the background\n",
                         getpid());
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
void treatCommand(struct cmdline *cmd) {
    char *cmdName = cmd->seq[0][0];
    if (!strcmp(cmdName, "cd")) {
        char *newDir = cmd->seq[0][1];
        changeDirectory(newDir);
    }
    else if (!strcmp(cmdName, "exit")) {
        exitShell(cmd);
    }
    else {
        execExternalCommand(cmd);
    }
}

int main() {
    // Main loop
    while (true) {
        // Show the prompt
        printf(PS1, getenv("USER"), getenv("PWD"));
        fflush(stdout);

        // Read a command from standard input and execute it
        cmd = readcmd();
        if (cmd == NULL) { // Exit if CTRL+D is pressed to avoid infinite loop
            DEBUG_PRINT("CTRL+D entered\n");
            exitShell(cmd);
        }
        else if (cmd->seq == NULL || *(cmd->seq) == NULL) { // Handle empty line
            DEBUG_PRINT("Empty line entered\n");
        }
        else {
            DEBUG_PRINTF("Treating command '%s'\n", cmd->seq[0][0]);
            treatCommand(cmd);
        }
    }
}