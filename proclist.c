#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "proclist.h"

void addProcess(t_proc *head, int pid, char *commandName) {
    DEBUG_PRINTF("Adding process %d to the list\n", pid);
    if (*head == NULL) { // The list is empty
        DEBUG_PRINT("List initialized\n");
        *head = createProcess(1, pid, commandName);
    }
    else {
        t_proc current = *head;
        // Reach the end of the list
        while (current->next != NULL) {
            current = current->next;
        }
        int newID = current->id + 1;
        current->next = createProcess(newID, pid, commandName);
    }
}

t_proc createProcess(int id, int pid, char *commandName) {
    t_proc newProc;
    DEBUG_PRINT("Allocating new process\n");
    newProc = (t_proc)malloc(sizeof(struct procList));
    if (newProc == NULL) {
        perror("malloc");
        exit(1);
    }

    newProc->id = id;
    newProc->pid = pid;
    newProc->state = ACTIVE;
    newProc->commandName = commandName;
    newProc->next = NULL;
    return newProc;
}

void removeProcess(t_proc *head, int id) {
    if (*head == NULL) {
        return; // Don't do anything if the list is empty
    }

    if ((*head)->id == id) {
        // Remove the first process of the list
        t_proc next = (*head)->next;
        free(*head);
        *head = next;
        return;
    }

    // Remove a process that is not the first in the list
    t_proc current = *head;
    while (current->next != NULL && current->next->id != id) {
        current = current->next;
    }
    if (current->next != NULL) {
        t_proc tmp = current->next;
        current->next = tmp->next;
        free(tmp);
    }
}

void printProcList(t_proc *head) {
    printf("\nProcess list:\n");
    if (*head == NULL) { // Empty list
        printf("\n");
        return;
    }

    // Loop trough each process in the list
    t_proc current = *head;
    while (current != NULL) {

        // Print the information about the process
        printf("ID: %d\n", current->id);
        printf("PID: %d\n", current->pid);
        printf("Command: %s\n", current->commandName);
        if (current->state == SUSPENDED) {
            printf("State: SUSPENDED\n");
        }
        else {
            printf("State: ACTIVE\n");
        }

        // Go to the next process
        current = current->next;
        printf("\n");
    }
    printf("\n");
}

void deleteProcList(t_proc *head) {
    t_proc current = *head;
    t_proc tmp = NULL;
    while (current != NULL) {
        tmp = current;
        current = current->next;
        free(tmp);
    }
    free(head);
}