/*
 * Built-in commands of the minishell
 */

#ifndef __BUILTINS_H
#define __BUILTINS_H

#include "readcmd.h"

/*
 * Function: changeDirectory
 * -------------------------
 *   Change the current directory, if an empty string is given,
 *   the current directory changed to the environment variable HOME
 */
void changeDirectory(char *newDir);

/*
 * Function: exitShell
 * -------------------
 *   Exit the shell
 *
 *   cmd: the last command read
 */
void exitShell(struct cmdline *cmd);

#endif