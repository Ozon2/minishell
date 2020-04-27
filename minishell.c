/*
 * A minimalist command line interface between the user and the operating system
 *
 * TODO:
 * - Handle SIGSTOP (use childHandler)
 * - CTRL-Z has unexpected results on background processes
 */

#define _GNU_SOURCE // WCONTINUED

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

// Global variables (used in signal handlers)
proc_t *procList;          // The process list
int foregroundPID = 0;     // PID of the foreground process
struct cmdline *cmd;       // The last command line entered
bool stopReceived = false; // CTRL+Z received by foreground process ?

/*
 * Function: execExternalCommand
 * -----------------------------
 *   Execute an external command, a subprocess will be forked
 *
 *   cmd: the command to execute
 */
void execExternalCommand(struct cmdline *cmd, proc_t *procList) {
    int forkPID;

    fflush(stdout);   // Flush stdout to give an empty buffer to the child process
    forkPID = fork(); // Make a child process to execute the command

    if (forkPID < 0) {
        perror("fork");
        exit(1);
    }
    else if (forkPID == 0) { // Child process

        DEBUG_PRINTF("[%d] Child process executing command '%s'\n", getpid(), cmd->seq[0][0]);
        execvp(cmd->seq[0][0], cmd->seq[0]);
        printf("Unknown command\n"); // If execvp returns, the command failed
        exit(1);
    }
    else { // Parent process
        if (cmd->backgrounded) {
            int newID = addProcess(procList, forkPID, ACTIVE, cmd->seq[0]);
            printProcessByID(procList, newID);
        }
        else {
            DEBUG_PRINTF("[%d] Parent process waiting for its child %d\n", getpid(), forkPID);
            foregroundPID = forkPID;
            // Wait for the child to finish or to be stopped,
            while (!stopReceived) {
            }
            stopReceived = false; // Reset stopReceived value
            foregroundPID = 0;
            DEBUG_PRINTF("[%d] Child %d stopped or ended\n", getpid(), forkPID);
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
    if (!strcmp(cmdName, "cd"))
        cd(cmd);
    else if (!strcmp(cmdName, "exit"))
        exitShell(procList);
    else if (!strcmp(cmdName, "list") || !strcmp(cmdName, "jobs"))
        list(procList);
    else if (!strcmp(cmdName, "stop"))
        stop(cmd, procList);
    else if (!strcmp(cmdName, "bg"))
        bg(cmd, procList);
    else if (!strcmp(cmdName, "fg"))
        fg(cmd, procList, &foregroundPID, &stopReceived);
    else
        execExternalCommand(cmd, procList);
}

/*
 * Function: childHandler
 * ----------------------
 *   Handle SIGCHLD
 */
void childHandler(int sig) {
    DEBUG_PRINTF("childHandler called with signal %d\n", sig);
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
                if (childPID == foregroundPID) {
                    DEBUG_PRINT("stopReceived=true\n");
                    stopReceived = true;
                }
                else {
                    setProcessStatusByPID(procList, childPID, SUSPENDED);
                }
            }
            else if (WIFCONTINUED(childState)) {
                DEBUG_PRINTF("[%d] Child resumed\n", childPID);
                setProcessStatusByPID(procList, childPID, ACTIVE);
            }
            else if (WIFEXITED(childState)) {
                DEBUG_PRINTF("[%d] Child exited, status=%d\n", childPID, WEXITSTATUS(childState));
                if (childPID == foregroundPID) {
                    DEBUG_PRINT("stopReceived=true\n");
                    stopReceived = true;
                    removeProcessByPID(procList, childPID);
                }
                else {
                    setProcessStatusByPID(procList, childPID, DONE);
                }
            }
            else if (WIFSIGNALED(childState)) {
                DEBUG_PRINTF("[%d] Child killed by signal %d\n", childPID, WTERMSIG(childState));
                if (childPID == foregroundPID) {
                    DEBUG_PRINT("stopReceived=true\n");
                    stopReceived = true;
                }
                else {
                    removeProcessByPID(procList, childPID);
                }
            }
        }
    } while (childPID > 0);
}

/*
 * Function: stopHandler
 * ---------------------
 *   Handle SIGTSTP
 */
void stopHandler() {
    if (foregroundPID == 0) {
        DEBUG_PRINT("Process 0 can't be suspended\n");
        return;
    }

    state status = getProcessStatusByPID(procList, foregroundPID);
    if (status == SUSPENDED) { // Process already stopped
        DEBUG_PRINTF("SIGTSTP received, but process %d is already suspended\n", foregroundPID);
        return;
    }
    DEBUG_PRINTF("SIGTSTP received, stopping foreground process %d\n", foregroundPID);
    kill(foregroundPID, SIGSTOP);
    // Add the process to the list
    if (status == UNDEFINED) { // If it is not already present in the list
        addProcess(procList, foregroundPID, SUSPENDED, cmd->seq[0]);
    }
    else {
        setProcessStatusByPID(procList, foregroundPID, SUSPENDED);
    }

    printProcessByPID(procList, foregroundPID);
}

int main() {
    // Associate signals to their handlers
    struct sigaction sa;
    sa.sa_handler = childHandler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, 0);

    sa.sa_handler = stopHandler;
    sigaction(SIGTSTP, &sa, 0);
    sigaction(SIGSTOP, &sa, 0);

    // // Mask
    // sigset_t ens_signaux;
    // sigemptyset(&ens_signaux);
    // sigaddset(&ens_signaux, SIGTSTP);
    // sigprocmask(SIG_BLOCK, &ens_signaux, NULL);

    // Create the process list
    procList = initProcList();

    // Main loop
    while (true) {
        // Show the prompt
        printf(PS1, getenv("USER"), getenv("PWD"));

        // Flush output buffer
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