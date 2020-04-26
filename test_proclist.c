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
    addProcess(head, 1279, ACTIVE, cmd1);
    addProcess(head, 1358, ACTIVE, cmd2);
    addProcess(head, 1423, ACTIVE, cmd3);
    addProcess(head, 1428, ACTIVE, cmd4);
    addProcess(head, 1507, ACTIVE, cmd5);
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

    fillList(head);
    printProcList(head);

    printf("Removing id 3\n");
    removeProcessByID(head, 3);
    printProcList(head);

    printf("Removing id 3 again\n");
    removeProcessByID(head, 3);
    printProcList(head);

    printf("Removing id 5\n");
    removeProcessByID(head, 5);
    printProcList(head);

    printf("Removing id 1\n");
    removeProcessByID(head, 1);
    printProcList(head);

    printf("Adding 5 more process\n");
    fillList(head);
    printProcList(head);
    deleteProcList(head);
}

void test_updateStatus() {
    printf("Test updateStatus\n");
    proc_t *head = initProcList();

    fillList(head);
    printProcList(head);

    printf("Set process 2 SUSPENDED\n");
    setProcessStatusByID(head, 2, SUSPENDED);
    printProcList(head);

    printf("Set process 1 DONE\n");
    setProcessStatusByID(head, 1, DONE);
    printProcList(head);

    printf("Set process 2 ACTIVE\n");
    setProcessStatusByID(head, 2, ACTIVE);
    printProcList(head);

    printf("Set process 5 SUSPENDED\n");
    setProcessStatusByID(head, 5, SUSPENDED);
    printProcList(head);

    printf("Set process 1 ACTIVE\n");
    setProcessStatusByID(head, 1, ACTIVE);
    printProcList(head);
    deleteProcList(head);
}

int main() {
    test_addProcess();
    test_removeProcess();
    test_updateStatus();
    return 0;
}