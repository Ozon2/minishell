/*
 * A minimalist command line interface between the user and the operating system
 */

#define _GNU_SOURCE // WCONTINUED

// Minishell prompt
#define PS1 "\033[0;33m%s\033[0;0m@\033[0;34mminishell\033[0m:\033[0;32m[%s]\033[0m$ "

#include <errno.h>
#include <fcntl.h>
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
 *   in: the input descriptor
 *   out: the output descriptor
 *   cmd: the command to execute
 *   i: the index of the command to execute in cmd
 *   procList: the process list
 */
void execExternalCommand(int in, int out, struct cmdline *cmd, int i, proc_t *procList) {
    int forkPID;

    fflush(stdout);   // Flush stdout to give an empty buffer to the child process
    forkPID = fork(); // Make a child process to execute the command

    if (forkPID < 0) {
        perror("fork");
        exit(1);
    }
    else if (forkPID == 0) { // Child process

        DEBUG_PRINTF("[%d] Child process executing command '%s'\n", getpid(), cmd->seq[i][0]);
        // Handle input redirection
        if (in != STDIN_FILENO) {
            DEBUG_PRINTF("Input read from %d\n", in);
            dup2(in, STDIN_FILENO);
            close(in);
        }

        // Handle output redirection
        if (out != STDOUT_FILENO) {
            DEBUG_PRINTF("Output redirected to %d\n", out);
            dup2(out, STDOUT_FILENO);
            close(out);
        }

        // We need to set the child process in its own group, otherwise
        // it will receive SIGTSTP when CTRL+Z is pressed
        setsid();
        execvp(cmd->seq[i][0], cmd->seq[i]);
        printf("Unknown command\n"); // If execvp returns, the command has failed
        exit(EXIT_FAILURE);
    }
    else { // Parent process
        if (cmd->backgrounded) {
            int newID = addProcess(procList, forkPID, ACTIVE, cmd->seq[i]);
            printProcessByID(procList, newID);
        }
        else {
            if (cmd->seq[i + 1] == NULL) { // Don't wait for piped processes
                DEBUG_PRINTF("[%d] Parent process waiting for its child %d\n", getpid(), forkPID);
                foregroundPID = forkPID;
                // Wait for the child to finish or to be stopped
                while (!stopReceived) {
                    sleep(0.1);
                }
                // Reset stopReceived and foregroundPID values
                stopReceived = false;
                foregroundPID = 0;
            }
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
        fg(cmd, procList, &foregroundPID, &stopReceived);
    }
    else {
        // Handle pipes and redirections
        int in, out, finalOutput, fd[2];

        // Open input file
        if (cmd->in != NULL) {
            in = open(cmd->in, O_RDONLY);
            if (in < 0) {
                printf("minishell: %s: No such file or directory\n", cmd->in);
                exit(EXIT_FAILURE);
            }
        }
        else {
            in = STDIN_FILENO;
        }

        // Open output file
        if (cmd->out != NULL) {
            finalOutput = open(cmd->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (finalOutput < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }
        }
        else {
            finalOutput = STDOUT_FILENO;
        }

        // Create a pipe between each consecutive process
        for (int i = 0; cmd->seq[i] != NULL; i++) {
            if (cmd->seq[i + 1] != NULL) {
                pipe(fd);
                out = fd[1];
            }
            else { // Redirect the last command
                out = finalOutput;
            }

            // in is assigned in the previous iteration
            execExternalCommand(in, out, cmd, i, procList);
            // Close the pipe because it is not used in the parent process
            if (out != STDOUT_FILENO && close(out) < 0) {
                perror("close output");
            }
            if (in != STDIN_FILENO && close(in) < 0) {
                perror("close input");
            }

            // The next child will read from the current pipe
            in = fd[0];
        }
    }
}

/*
 * Function: childHandler
 * ----------------------
 *   Handle SIGCHLD
 */
void childHandler() {
    DEBUG_PRINT("childHandler received a signal\n");
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
                    // Add the process to the list if it is not already present in the list
                    if (getProcessStatusByPID(procList, foregroundPID) == UNDEFINED) {
                        addProcess(procList, foregroundPID, SUSPENDED, cmd->seq[0]);
                    }
                    setProcessStatusByPID(procList, childPID, SUSPENDED);
                    printProcessByPID(procList, childPID);
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
        DEBUG_PRINTF("Stop signal received, but process %d is already suspended\n", foregroundPID);
        return;
    }
    DEBUG_PRINTF("Stop signal received, stopping foreground process %d\n", foregroundPID);
    kill(foregroundPID, SIGSTOP);
}

/*
 * Function: sigintHandler
 * -----------------------
 *   Handle SIGINT
 */
void sigintHandler() {
    if (foregroundPID == 0) {
        DEBUG_PRINT("Process 0 can't be interrupted\n");
        return;
    }
    DEBUG_PRINTF("SIGINT received, interrupting foreground process %d\n", foregroundPID);
    kill(foregroundPID, SIGKILL);
    stopReceived = true;
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

    sa.sa_handler = sigintHandler;
    sigaction(SIGINT, &sa, 0);

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

        if (cmd == NULL) { // Exit if CTRL+D is pressed to avoid an infinite loop
            DEBUG_PRINT("CTRL+D entered, exiting ...\n");
            exitShell(procList);
        }
        else if (cmd->seq == NULL || *(cmd->seq) == NULL) { // Handle empty line
            DEBUG_PRINT("Empty line entered\n");
        }
        else {
            // Treat the command
            DEBUG_PRINTF("Treating command '%s'\n", cmd->seq[0][0]);
            treatCommand(cmd, procList);
        }
    }
}