# Makefile for the project.

BIN:=./bin
OBJ:=./obj
SRC:=./src
DIRS:=$(BIN) $(OBJ) $(SRC)

CC:=gcc
CCOPTS:=-Wall -Wextra -pedantic -g -c


.PHONY: clean

# Makefile targets

all: $(DIRS)


# Clean up!
clean:
	# rm -rf -- $(BIN)/*
	# rm -rf -- $(OBJ)/*

# How to build an executable
# $(BIN)/%:$(OBJ)/%.o
# 	$(CC) -o $@ $<

# How to compile a C file
$(OBJ)/%.o:$(SRC)/%.c
	$(CC) $(CCOPTS) -o $@ $<

$(DIRS):
	mkdir -pv $@
