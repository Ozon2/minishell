#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "proclist.h"

// Maximum size of a command line to keep in the process list
#define MAX_NAME_SIZE 30

proc_t *initProcList() {
    proc_t *head = safe_malloc(sizeof(head));
    *head = NULL;
    return head;
}

int addProcess(proc_t *head, int pid, char **commandName) {
    DEBUG_PRINTF("Adding process %d to the list\n", pid);
    if (*head == NULL) { // The list is empty
        DEBUG_PRINT("List initialized\n");
        *head = createProcess(1, pid, commandName);
        return 1;
    }
    else {
        proc_t current = *head;
        // Reach the end of the list
        while (current->next != NULL) {
            current = current->next;
        }
        int newID = current->id + 1;
        current->next = createProcess(newID, pid, commandName);
        return newID;
    }
}

proc_t createProcess(int id, int pid, char **commandName) {
    proc_t newProc;
    DEBUG_PRINT("Allocating new process\n");
    newProc = safe_malloc(sizeof(struct procList));

    // Copy the command line
    char *name = safe_malloc(MAX_NAME_SIZE);
    strcpy(name, *commandName);
    char **ptr = commandName;
    for (char *c = *++ptr; c != NULL; c = *++ptr) {
        if (strlen(name) + strlen(c) + 4 < MAX_NAME_SIZE) {
            strcat(name, " ");
            strcat(name, c);
        }
        else {
            DEBUG_PRINT("Command name too long\n");
            break;
        }
    }
    strcat(name, " &");

    newProc->id = id;
    newProc->pid = pid;
    newProc->state = ACTIVE;
    newProc->commandName = name;
    gettimeofday(&(newProc->time), NULL);
    newProc->next = NULL;
    return newProc;
}

int lengthProcList(proc_t *head) {
    proc_t current = *head;
    int n = 0;
    while (current != NULL) {
        n++;
        current = current->next;
    }
    return n;
}

void removeProcess(proc_t *head, int id) {
    if (*head == NULL) {
        return; // Don't do anything if the list is empty
    }

    if ((*head)->id == id) {
        // Remove the first process of the list
        proc_t next = (*head)->next;
        free((*head)->commandName);
        free(*head);
        *head = next;
        return;
    }

    // Remove a process that is not the first in the list
    proc_t current = *head;
    while (current->next != NULL && current->next->id != id) {
        current = current->next;
    }
    if (current->next != NULL) {
        proc_t tmp = current->next;
        current->next = tmp->next;
        free(tmp->commandName);
        free(tmp);
    }
}

void printProcList(proc_t *head) {
    printf("\nProcess list:\n");
    if (*head == NULL) { // Empty list
        printf("\n");
        return;
    }

    // Loop trough each process in the list
    proc_t current = *head;
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
        // Temps en microsecondes
        printf("Time: %luus\n", current->time.tv_sec * 1000000 + current->time.tv_usec);

        // Go to the next process
        current = current->next;
        printf("\n");
    }
    printf("\n");
}

void getTwoLastProcesses(proc_t *head, int *lastID, int *previousID) {
    proc_t current = *head;
    proc_t last = *head;
    *lastID = 0;
    proc_t previous = *head;
    *previousID = 0;
    while (current != NULL) {
        // current time > last time
        if (current->time.tv_sec >= last->time.tv_sec &&
            current->time.tv_usec > last->time.tv_usec) {
            // Update previous and last
            previous = last;
            *previousID = last->id;
            last = current;
            *lastID = current->id;
        }
        // current time > previous time
        else if (current->time.tv_sec >= previous->time.tv_sec &&
                 current->time.tv_usec > previous->time.tv_usec) {
            // Update previous
            previous = current;
            *previousID = current->id;
        }
        current = current->next;
    }
}

void deleteProcList(proc_t *head) {
    proc_t current = *head;
    proc_t tmp = NULL;
    while (current != NULL) {
        tmp = current;
        current = current->next;
        free(tmp->commandName);
        free(tmp);
    }
    free(head);
}