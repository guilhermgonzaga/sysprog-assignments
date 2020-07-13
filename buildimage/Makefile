# Makefile for the project.
# Best viewed with tabs set to 4 spaces.

CC:=gcc
LD:=ld

BIN:=./bin
OBJ:=./obj
SRC:=./src

# Where to locate the kernel in memory
KERNEL_ADDR:=0x1000

# Compiler flags
#-fno-builtin:		Don't recognize builtin functions that do not begin
#			with '__builtin_' as prefix.
#
#-fomit-frame-pointer:	Don't keep the frame pointer in a register for
#			functions that don't need one.
#
#-make-program-do-what-i-want-it-to-do:
#			Turn on all friendly compiler flags.
#
#-O2:			Turn on all optional optimizations except for loop
#			unrolling and function inlining.
#
#-c:			Compile or assemble the source files, but do not link.
#
#-Wall:			All of the `-W' options combined (all warnings on)

CCOPTS = -Wall -g -m32 -c -fomit-frame-pointer -O2 -fno-builtin -pedantic

# Linker flags
#-nostartfiles:	Do not use the standard system startup files when linking.
#
#-nostdlib:	Don't use the standard system libraries and startup files
#		when linking. Only the files you specify will be passed
#		to the linker.
#

LDOPTS = -nostartfiles -nostdlib -melf_i386


.PHONY: clean

# Makefile targets

all: kernel bootblock buildimage image
kernel: $(BIN)/kernel
bootblock: $(BIN)/bootblock
buildimage: $(BIN)/buildimage
image: $(BIN)/image

# Put the image on the usb stick (these two stages are independent, as both
# vmware and bochs can run using only the image file stored on the harddisk)
boot: $(BIN)/image
	dd if=$< of=/dev/sdb bs=512

# Clean up!
# Cannot delete bootblock.o or buildimage.given!
clean:
	cd $(OBJ) && rm -f buildimage.o kernel.o
	cd $(BIN) && rm -f buildimage kernel image bootblock

# No, really, clean up!
distclean: clean
	rm -f -- *~
	rm -f -- \#*
	rm -f -- *.bak
	rm -f serial.out bochsout.txt


# How to build the kernel
$(BIN)/kernel: $(OBJ)/kernel.o
	$(LD) $(LDOPTS) -Ttext $(KERNEL_ADDR) -o $@ $<

# How to build bootblock
$(BIN)/bootblock: $(OBJ)/bootblock.o
	$(LD) $(LDOPTS) -Ttext 0x0 -o $@ $<

# How to build buildimage
$(BIN)/buildimage: $(OBJ)/buildimage.o
	$(CC) -o $@ $<

# Build an image to put on the floppy
$(BIN)/image: $(BIN)/buildimage $(BIN)/bootblock $(BIN)/kernel
	cd $(BIN) && ./buildimage --extended bootblock kernel

# How to compile buildimage
$(OBJ)/buildimage.o:
	$(CC) -c -g -Wall -pedantic -o $@ $(SRC)/buildimage.c

# How to compile a C file
$(OBJ)/%.o:$(SRC)/%.c
	$(CC) $(CCOPTS) -o $@ $<

# How to assemble
$(OBJ)/%.o:$(SRC)/%.s
	$(CC) $(CCOPTS) -o $@ $<

# How to produce assembler input from a C file
$(SRC)/%.s:$(SRC)/%.c
	$(CC) $(CCOPTS) -S -o $@ $<
