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
 *
 *   cmd: the command line
 */
void cd(struct cmdline *cmd);

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

/*
 * Function: stop
 * --------------
 *   Suspend a process
 *
 *   cmd: the command line
 *   procList: the process list
 */
void stop(struct cmdline *cmd, proc_t *procList);

/*
 * Function: bg
 * ------------
 *   Resume a background process
 *
 *   cmd: the command line
 *   procList: the process list
 */
void bg(struct cmdline *cmd, proc_t *procList);

/*
 * Function: fg
 * ------------
 *   Take to foreground and resume a background process
 *
 *   cmd: the command line
 *   procList: the process list
 */
void fg(struct cmdline *cmd, proc_t *procList);

#endif