/*
 * Linked list to store the processes
 */

#ifndef __PROCLIST_H
#define __PROCLIST_H

// Define the state of a process
typedef enum state { SUSPENDED, ACTIVE } state;

typedef struct procList {
    int id;                // ID of the process in the minishell
    int pid;               // Process ID
    state state;           // Current state of the process
    char *commandName;     // Name of the command executed by this process
    struct procList *next; // Next process in the list
} * t_proc;

/*
 * Function: createProcess
 * --------------------
 *   Create a new process list
 *
 *   id: the ID of the process in the minishell
 *   pid: the process ID
 *   commandName: the name of the command executed by this process
 */
t_proc createProcess(int id, int pid, char *commandName);

/*
 * Function: addProcess
 * --------------------
 *   Add a process to the list
 *
 *   head: the head of the list
 *   pid: the process ID
 *   commandName: the name of the command executed by this process
 */
void addProcess(t_proc *head, int pid, char *commandName);

/*
 * Function: removeProcess
 * -----------------------
 *   Remove a process from the list, if the process is not present
 *   in the list, the function does nothing
 *
 *   head: the head of the list
 *   id: ID of the process to remove
 */
void removeProcess(t_proc *head, int id);

/*
 * Function: printProcList
 * -----------------------
 *   Print the process list
 *
 *   head: the head of the list
 */
void printProcList(t_proc *head);

/*
 * Function: deleteProcList
 * ------------------------
 *   Delete the process list
 *
 *   head: the head of the list
 */
void deleteProcList(t_proc *head);

#endif