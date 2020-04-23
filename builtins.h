/*
 * Built-in commands of the minishell
 */

#ifndef __BUILTINS_H
#define __BUILTINS_H

#include "proclist.h"
#include "readcmd.h"

/*
 * Function: cd
 * ------------
 *   Change the current directory, if an empty string is given,
 *   the current directory changed to the environment variable HOME
 */
void cd(char *newDir);

/*
 * Function: exitShell
 * -------------------
 *   Exit the shell
 *
 *   procList: the process list
 */
void exitShell(proc_t *procList);

/*
 * Function: list
 * --------------
 *   Display the suspended and active processes
 *
 *   Example:
 *     [1]   Running                 sleep 1000 &
 *     [2]-  Running                 sleep 1000 &
 *     [3]+  Running                 sleep 1000 &
 *
 *   Notes: '+' and '-' are displayed respectively for the last and
 *   the second-to-last modified processes
 *
 *   procList: the process list
 */
void list(proc_t *procList);

#endif