/*
 * Linked list to store the processes
 */

#ifndef __PROCLIST_H
#define __PROCLIST_H

#include <sys/time.h>

// Define the state of a process
typedef enum state { SUSPENDED, ACTIVE, DONE } state;

// Struct to define a process
typedef struct procList {
    int id;                // ID of the process in the minishell
    int pid;               // Process ID
    state state;           // Current state of the process
    char *commandName;     // Name of the command executed by this process
    struct timeval time;   // Time at which the process state was last modified
    struct procList *next; // Next process in the list
} * proc_t;

/*
 * Function: initProcList
 * --------------------
 *   Initialize the process list
 *
 *   id: the ID of the process in the minishell
 *   pid: the process ID
 *   commandName: the name of the command executed by this process
 *
 *   Return: the head of the created process list
 */
proc_t *initProcList();

/*
 * Function: createProcess
 * --------------------
 *   Create a new process list
 *
 *   id: the ID of the process in the minishell
 *   pid: the process ID
 *   commandName: the name of the command executed by this process
 *
 *   Return: the created process
 */
proc_t createProcess(int id, int pid, char **commandName);

/*
 * Function: addProcess
 * --------------------
 *   Add a process to the list
 *
 *   head: a pointer to the the head of the list
 *   pid: the process ID
 *   commandName: the name of the command executed by this process
 *
 *   Return: ID of the new process in the minishell
 */
int addProcess(proc_t *head, int pid, char **commandName);

/*
 * Function: lengthProcList
 * ------------------------
 *   Get the length of the process list
 *
 *   head: a pointer to the the head of the list
 *
 *   Returns: the length of the process list
 */
int lengthProcList(proc_t *head);

/*
 * Function: removeProcessByID
 * ---------------------------
 *   Remove a process from the list, if the process is not present
 *   in the list, the function does nothing
 *
 *   head: a pointer to the the head of the list
 *   id: ID of the process to remove
 */
void removeProcessByID(proc_t *head, int id);

/*
 * Function: removeProcessByPID
 * ----------------------------
 *   Remove a process from the list, if the process is not present
 *   in the list, the function does nothing
 *
 *   head: a pointer to the the head of the list
 *   pid: PID of the process to remove
 */
void removeProcessByPID(proc_t *head, int id);

/*
 * Function: printProcess
 * ----------------------
 *   Print the information about a process
 *
 *   head: a pointer to the the head of the list
 *   lastID: ID of the last modified process
 *   previousID: ID of the second-to-last modified process
 */
void printProcess(proc_t proc, int lastID, int previousID);

/*
 * Function: printProcList
 * -----------------------
 *   Print the process list
 *
 *   head: a pointer to the the head of the list
 */
void printProcList(proc_t *head);

/*
 * Function: getLastTwoProcesses
 * -----------------------------
 *   Get the last two processes from the process list
 *
 *   head: a pointer to the the head of the list
 *   lastID: (out) the ID of the last modified process found (0 if not found)
 *   previousID: (out) the ID of the second-to-last modified process found (0 if not found)
 */
void getTwoLastProcesses(proc_t *head, int *lastID, int *previousID);

/*
 * Function: changeStatus
 * ----------------------
 *   Change the status of a process
 *
 *   head: a pointer to the the head of the list
 *   pid: the PID of the process
 *   status: the new status
 */
void changeStatus(proc_t *head, int pid, state status);

/*
 * Function: updateProcList
 * ------------------------
 *   Print and remove from the list the completed processes
 *
 *   head: a pointer to the the head of the list
 */
void updateProcList(proc_t *head);

/*
 * Function: deleteProcList
 * ------------------------
 *   Delete the process list
 *
 *   head: a pointer to the the head of the list
 */
void deleteProcList(proc_t *head);

#endif