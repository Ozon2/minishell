#include <stdio.h>
#include <stdlib.h>

#include "proclist.h"

void fillList(t_proc *head) {
    addProcess(head, 1279, "ls");
    addProcess(head, 1358, "sleep");
    addProcess(head, 1423, "echo");
    addProcess(head, 1428, "cat");
    addProcess(head, 1507, "vim");
}

void test_addProcess() {
    printf("Test addProcess\n");
    t_proc *head = (t_proc *)malloc(sizeof(head));
    if (head == NULL) {
        perror("malloc");
        exit(1);
    }
    *head = NULL;

    fillList(head);
    printProcList(head);
    deleteProcList(head);
}

void test_removeProcess() {
    printf("Test removeProcess\n");
    t_proc *head = (t_proc *)malloc(sizeof(head));
    if (head == NULL) {
        perror("malloc");
        exit(1);
    }
    *head = NULL;

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