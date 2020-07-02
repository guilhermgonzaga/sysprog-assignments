# Makefile for the project.
# Best viewed with tabs set to 4 spaces.

BIN:=./bin
LIB:=./lib
OBJ:=./obj
SRC:=./src
DIRS:=$(BIN) $(LIB) $(OBJ) $(SRC)

CC:=gcc
CCOPTS:=-Wall -g -c -pedantic -I$(SRC)


.PHONY: clean
.SECONDARY: $(OBJ)/bitest.o $(OBJ)/fibtest.o $(OBJ)/libbi.o

# Makefile targets

all: $(DIRS) bitest fibtest libbi
bitest: $(DIRS) $(BIN)/bitest
fibtest: $(DIRS) $(BIN)/fibtest
libbi: $(DIRS) $(LIB)/libbi.a

# Clean up!
clean:
	rm -f -- $(OBJ)/* $(BIN)/*

# No, really, clean up!
distclean: clean
	rm -rf -- $(OBJ) $(BIN)
	rm -f -- *~
	rm -f -- \#*
	rm -f -- *.bak


# How to link an object to libbi
$(BIN)/%: $(OBJ)/%.o $(LIB)/libbi.a
	$(CC) -L$(LIB) -o $@ $< -lbi

# How to create a static library from object file
$(LIB)/%.a: $(OBJ)/%.o
	ar rcs $@ $<

# How to compile a C file
$(OBJ)/%.o:$(SRC)/%.c
	$(CC) $(CCOPTS) -o $@ $<

# How to assemble
$(OBJ)/%.o:$(SRC)/%.s
	$(CC) $(CCOPTS) -o $@ $<

# How to produce assembler input from a C file
$(SRC)/%.s:$(SRC)/%.c
	$(CC) $(CCOPTS) -S -o $@ $<

$(DIRS):
	mkdir -p $@
