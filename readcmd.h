#ifndef __READCMD_H
#define __READCMD_H

/* Introduction
------------
Le code fourni a pour but de vous décharger du travail d'analyse d'une ligne de commande,
avant son interprétation par le shell. Il propose une fonction `readcmd()` qui fournit
le résultat de cette analyse.

Le source est fourni non pas à titre d'exemple (il n'est pas d'une clarté éblouissante),
mais à titre de documentation (et de spécification :)) ;  il n'est ni
nécessaire, ni conseillé de le modifier pour réaliser le travail demandé : l'analyse
réalisée est suffisante pour la syntaxe des commandes que votre shell doit interpréter.

Ce code est a priori tout à fait fiable, mais n'hésitez cependant pas à nous signaler 
ce qui vous apparaîtrait comme des anomalies, ou des lacunes dans la documentation.

La structure `cmdline`
----------------------
L'appel à readcmd() retourne une structure cmdline, qui contient le résultat de l'analyse
de la ligne de commande pour le shell.
Cette structure contient notamment :

- l'indication du fait que la commande doit être lancée en tâche de fond ou non
- les redirections éventuelles
- la décomposition de la ligne de commande en commandes élémentaires, et la décomposition
 de chaque commande en mots. Le champ `seq` référence le résultat de cette décomposition,
 qui est vu comme un tableau à deux dimensions [commandes élémentaires, mots de la commande]
 
 **Pour plus de détails, voir le fichier d'en-tête `readcmd.h`.**
 
**Exemples :**

- "ls -l" : seq[0][0] = "ls", seq[0][1] = "-l", seq[0][2] = NULL, seq[1] = NULL, backgrounded = NULL, in = NULL, out = NULL
- "ls -l > toto" : seq[0][0] = "ls", seq[0][1] = "-l", seq[0][2] = NULL,
 seq[1] = NULL, backgrounded = NULL, in = NULL, out => "toto"
- "ls | grep toto | wc -l" : seq[0][0] = "ls", seq[0][1] = NULL, 
seq[1][0] = "grep", seq[1][1] = "toto",  seq[1][2] = NULL, 
seq[2][0] = "wc", seq[0][1] = "-l", seq[0][2] = NULL,
seq[3] = NULL, backgrounded = NULL, in = NULL, out = NULL
- "sleep 100 &" : seq[0][0] = "sleep", seq[0][1] = "20",  backgrounded != NULL, in = NULL, out = NULL */

/* Lit une ligne de commande depuis l'entrée standard.
 * Remarque :
 * Dans le cas où suffisamment de mémoire ne peut être allouée pour le résultat,
 * affiche un message d'erreur et appelle exit().
 */
struct cmdline* readcmd(void);

/* Structure retournée par readcmd() */
struct cmdline {
    char* err; /* Si non null : message d'erreur à afficher. 
				 * Dans ce cas, les autres champs sont nuls. */
    char* in; /* Si non null : nom du fichier vers lequel l'entrée doit être redirigée. */
    char* out; /* Si non null : nom du fichier vers lequel la sortie doit être redirigée. */
    char* backgrounded; /* Si non null : commande en tâche de fond */
    char*** seq; /* Une ligne de commande est une suite de commandes liées par des tubes
					 * Ainsi, 
					 * - une commande est un tableau de chaînes de caractères (char **).
                     *   Chaque élément de ce tableau est en effet une chaîne (char *)
                     *   correspondant à un argument de la commande.
					 *   Le dernier pointeur du tableau est suivi d'un pointeur null, qui
                     *   marque la fin du tableau.
					 * - seq est un tableau de commandes (char ***), chaque commande de ce
                     *   tableau correspondant à une commande élémentaire du pipeline.
					 *   Le dernier élément (char **) du tableau est ici aussi suivi d'un pointeur null.
					 * - Dans le cas particulier où une ligne vide est saisie, le pointeur référencé par
                     *   seq vaut directement NULL : puisque le tableau n'a aucun élément, le marqueur
                     *   de fin de tableau se trouve référencé par seq, ce que l'on peut écrire 
                     *   (*seq == NULL) 
                     *   (ou (seq[0] == NULL), mais cela peut prêter à confusion, seq[0] n'existant pas)
					 */
};
#endif
