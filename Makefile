CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g3
LDFLAGS=-fsanitize=address
EXEC=minishell

minishell: readcmd.o builtins.o proclist.o minishell.o 
	$(CC) $(LDFLAGS) $^ -o $@

depend:
	makedepend *.c -Y.

clean:
	rm *.o $(EXEC)

.PHONY: depend clean 

# DO NOT DELETE

builtins.o: builtins.h readcmd.h debug.h
minishell.o: builtins.h readcmd.h debug.h proclist.h
proclist.o: debug.h proclist.h
readcmd.o: readcmd.h
test_proclist.o: proclist.h