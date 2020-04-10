#define DEBUG 1

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, ...)                                                                      \
    fprintf(stderr, "[DEBUG]: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) // Don't do anything if not in debug mode
#endif

#include "readcmd.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

bool isBuiltin(struct cmdline *cmd) {
    char *cmd_name = cmd->seq[0][0];
    return (strcmp(cmd_name, "cd") == 0) || (strcmp(cmd_name, "exit") == 0);
}

void execBuiltin(struct cmdline *cmd) {
    DEBUG_PRINT("Executing built-in command '%s'\n", cmd->seq[0][0]);
    execvp(cmd->seq[0][0], cmd->seq[0]);
}

void execCommand(struct cmdline *cmd) {
    int child_pid, wstatus;
    child_pid = fork(); // Make a child process to execute the command

    if (child_pid < 0) {

        perror("fork");
        exit(1);
    }
    else if (child_pid == 0) { // Child process

        DEBUG_PRINT("Executing command '%s' by process of pid %d\n", cmd->seq[0][0], getpid());
        execvp(cmd->seq[0][0], cmd->seq[0]);
        printf("Unknown command\n"); // If execvp returns, it has failed
        exit(1);
    }
    else { // Parent process

        wait(&wstatus); // Wait for the child to display the command's results
    }
}

int main() {
    struct cmdline *cmd;

    // Main loop
    while (true) {
        printf("sh$ ");
        cmd = readcmd();   // Read a line from standard input
        if (cmd == NULL) { // Exit if CTRL+D is pressed to avoid infinite loop
            exit(0);
        }
        else if (*(cmd->seq) == NULL) // Handle empty line
            continue;
        else if (isBuiltin(cmd))
            execBuiltin(cmd);
        else
            execCommand(cmd);
    }
}