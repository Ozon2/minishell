#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINTF(fmt, ...)                                                                     \
    fprintf(stderr, "\033[0;31m[DEBUG]\033[0m: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,    \
            __VA_ARGS__)
#define DEBUG_PRINT(fmt)                                                                           \
    fprintf(stderr, "\033[0;31m[DEBUG]\033[0m: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__)
#else
#define DEBUG_PRINTF(fmt, ...) // Don't do anything if not in debug mode
#define DEBUG_PRINT(fmt)
#endif

#include "readcmd.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Minishell prompt
#define PS1 "\033[0;33m%s\033[0;0m@\033[0;34mminishell\033[0m:\033[0;32m[%s]\033[0m$ "

struct cmdline *cmd;

/*
 * Execute an external command, a subprocess will be forked
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
            if
                WIFEXITED(wstatus) { /* fils terminé avec exit */
                    DEBUG_PRINTF("[%d] Child process ended with exit %d\n", getpid(),
                                 WEXITSTATUS(wstatus));
                }
            else if
                WIFSIGNALED(wstatus) { /* fils tué par un signal */
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
 * Change the current directory
 * If an empty string is given, change the current directory to the environment variable HOME
 */
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

/*
 * Exit the shell
 */
void exitShell(struct cmdline *cmd) {
    freecmd(cmd);
    free(cmd);
    exit(0);
}

/*
 * Treat a given command
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
        printf(PS1, getenv("USER"), getenv("PWD"));
        cmd = readcmd();   // Read a line from standard input
        if (cmd == NULL) { // Exit if CTRL+D is pressed to avoid infinite loop
            DEBUG_PRINT("CTRL+D entered, exiting ...\n");
            exitShell(cmd);
        }
        else if (*(cmd->seq) == NULL) { // Handle empty line
            DEBUG_PRINT("Empty line entered\n");
        }
        else {
            DEBUG_PRINTF("Treating command '%s'\n", cmd->seq[0][0]);
            treatCommand(cmd);
        }
    }
}