CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS =

all: minishell

minishell: readcmd.o minishell.o 
	$(CC) $(LDFLAGS) $^ -o $@

depend:
	makedepend *.c -Y.

clean:
	rm *.o minishell

.PHONY: all depend clean 

# DO NOT DELETE

minishell.o: readcmd.h
readcmd.o: readcmd.h
