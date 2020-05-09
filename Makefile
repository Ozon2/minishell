CC=gcc
# Some debug flags to check for memory leaks, and undefined behaviour
DEBUG=-g3 -fno-omit-frame-pointer -fsanitize=address,undefined,leak,unreachable,null,bounds
CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=
EXEC=minishell test test_fg

all: minishell test test_fg

minishell: readcmd.o builtins.o proclist.o debug.o minishell.o
	$(CC) $(LDFLAGS) $^ -o $@

test: proclist.o debug.o test_proclist.o
	$(CC) $(LDFLAGS) $^ -o $@

test_fg: test_fg.o
	$(CC) $(LDFLAGS) $^ -o $@

depend:
	makedepend *.c -Y.

clean:
	rm *.o $(EXEC)

.PHONY: depend clean all

# DO NOT DELETE

builtins.o: builtins.h proclist.h readcmd.h debug.h
debug.o: debug.h
minishell.o: builtins.h proclist.h readcmd.h debug.h
proclist.o: debug.h proclist.h
readcmd.o: readcmd.h
test_proclist.o: proclist.h
