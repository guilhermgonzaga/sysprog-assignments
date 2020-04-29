# Makefile for the project.
# Best viewed with tabs set to 4 spaces.

CC = gcc

CCOPTS = -Wall -g -c -I.

# Makefile targets

all: fibtest bitest libbi.a

libbi.a: libbi.o
	ar rcs libbi.a libbi.o

bitest: bitest.o libbi.a
	$(CC) -L. -o bitest bitest.o -lbi

fibtest: fibtest.o libbi.a
	$(CC) -L. -o fibtest fibtest.o -lbi

# Clean up!
clean:
	@rm -fv fibtest.o fibtest bitest.o bitest libbi.o libbi.a

# No, really, clean up!
distclean: clean
	@rm -fv *~
	@rm -fv \#*
	@rm -fv *.bak

# How to compile a C file
%.o:%.c
	$(CC) $(CCOPTS) $<

# How to assemble
%.o:%.s
	$(CC) $(CCOPTS) $<

# How to produce assembler input from a C file
%.s:%.c
	$(CC) $(CCOPTS) -S $<
