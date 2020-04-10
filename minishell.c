#define DEBUG 1

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, ...)                                                                      \
    fprintf(stderr, "[DEBUG]: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) /* Don't do anything if not in debug mode */
#endif

#include "readcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    struct cmdline *cmd;
    int child_pid, wstatus;

    // Main loop
    while (1) {
        printf("sh$ ");

        // Read a line from standard input
        cmd = readcmd();

        // Make a child process to execute the command
        child_pid = fork();
        if (child_pid < 0) {
            perror("fork");
            exit(1);
        }
        else if (child_pid == 0) { // Child process

            DEBUG_PRINT("Process %d executed command %s\n", getpid(), cmd->seq[0][0]);
            execvp(cmd->seq[0][0], cmd->seq[0]);

            printf("Unknown command\n"); // If execvp returns, it has failed
            exit(1);
        }
        else { // Parent process

            wait(&wstatus); // Wait for the child to display the command's results
        }
    }
}