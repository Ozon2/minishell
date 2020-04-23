#include <stdio.h>
#include <stdlib.h>

#include "proclist.h"

void fillList(proc_t *head) {
    char *cmd1[] = {"ls", "-l", NULL};
    char *cmd2[] = {"echo", "2", NULL};
    char *cmd3[] = {"sleep", "1000", NULL};
    char *cmd4[] = {"cat", "proclist.h", NULL};
    char *cmd5[] = {"vim", "minishell.c", "this command is too long and will exceed the max size",
                    NULL};
    addProcess(head, 1279, cmd1);
    addProcess(head, 1358, cmd2);
    addProcess(head, 1423, cmd3);
    addProcess(head, 1428, cmd4);
    addProcess(head, 1507, cmd5);
}

void test_addProcess() {
    printf("Test addProcess\n");
    proc_t *head = initProcList();
    if (head == NULL) {
        perror("malloc");
        exit(1);
    }

    fillList(head);
    printProcList(head);
    deleteProcList(head);
}

void test_removeProcess() {
    printf("Test removeProcess\n");
    proc_t *head = initProcList();
    if (head == NULL) {
        perror("malloc");
        exit(1);
    }

    fillList(head);
    printProcList(head);

    printf("Removing id 3\n");
    removeProcess(head, 3);
    printProcList(head);

    printf("Removing id 3 again\n");
    removeProcess(head, 3);
    printProcList(head);

    printf("Removing id 5\n");
    removeProcess(head, 5);
    printProcList(head);

    printf("Removing id 1\n");
    removeProcess(head, 1);
    printProcList(head);

    printf("Adding 5 more process\n");
    fillList(head);
    printProcList(head);
    deleteProcList(head);
}

int main() {
    test_addProcess();
    test_removeProcess();
    return 0;
}