CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g
LDFLAGS=
EXEC=minishell

minishell: readcmd.o minishell.o 
	$(CC) $(LDFLAGS) $^ -o $@

depend:
	makedepend *.c -Y.

clean:
	rm *.o $(EXEC)

.PHONY: depend clean 

# DO NOT DELETE

minishell.o: readcmd.h
readcmd.o: readcmd.h
