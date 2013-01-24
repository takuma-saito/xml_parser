# Makefile
CC = gcc
CFLAGS = -g -Wall
OBJS = base.o io.o dot.o term.o  xml-parser.o
PROG = xp

.SUFFIXES: .c .o

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $^

.c.o:
	$(CC) $(CFLAGS) -c $^

.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJS)
