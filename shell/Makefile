# Makefile for the project.

BIN:=./bin
INCLUDE:=./include
OBJ:=./obj
SRC:=./src
TEST:=./test

DIRS:=$(BIN) $(INCLUDE) $(OBJ) $(SRC) $(TEST)
SOURCES:=$(wildcard $(SRC)/*.c)
# TESTS:=$(wildcard $(TEST)/*.c)

CC:=gcc
CCOPTS:=-Wall -Wextra -pedantic -I$(INCLUDE) -g -c


.PHONY: clean

# Makefile targets

all: $(DIRS) $(BIN)/shell


# Clean up!
clean:
	rm -rf -- $(BIN)/*
	rm -rf -- $(OBJ)/*


# How to build the shell
$(BIN)/shell: $(SOURCES:$(SRC)%.c=$(OBJ)%.o)
	$(CC) -o $@ $^


# How to compile a C file
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CCOPTS) -o $@ $<


$(DIRS):
	mkdir -p $@
