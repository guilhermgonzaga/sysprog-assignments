# Makefile for the project.
# Best viewed with tabs set to 4 spaces.

BIN:=./bin
OBJ:=./obj
SRC:=./src
DIRS:=$(BIN) $(OBJ) $(SRC)

CC:=gcc
CCOPTS:=-Wall -g -c -I$(SRC)


.PHONY: clean
.SECONDARY: $(OBJ)/bitest.o $(OBJ)/fibtest.o $(OBJ)/libbi.o

# Makefile targets

all: $(DIRS) bitest fibtest libbi
bitest: $(DIRS) $(BIN)/bitest
fibtest: $(DIRS) $(BIN)/fibtest
libbi: $(DIRS) $(BIN)/libbi.a

# Clean up!
clean:
	@rm -fv -- $(OBJ)/* $(BIN)/*

# No, really, clean up!
distclean: clean
	@rm -rfv -- $(OBJ) $(BIN)
	@rm -fv -- *~
	@rm -fv -- \#*
	@rm -fv -- *.bak


# How to link an object to libbi
$(BIN)/%: $(OBJ)/%.o $(BIN)/libbi.a
	$(CC) -L$(BIN) -o $@ $< -lbi

# How to create a static library from object file
$(BIN)/%.a: $(OBJ)/%.o
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
	@mkdir -pv $@
