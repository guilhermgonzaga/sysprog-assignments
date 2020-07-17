# libbi: x86-64 assembly implementation for fixed-width big integer arithmetic

This project was built to learn x86-64 assembly control flow and data manipulation by writing a big integer arithmetic library. The implementation is in `libbi.s`, and a header file, `BigInt.h`, is available.


## Supported operations

1. Assignment by copy
1. Two's complement (invert sign)
1. Addition
1. Subtraction
1. Multiplication
1. Equality and relational
1. Bitwise operations (same as in C)
1. Shift left and right (arithmetic)
	- There's a caveat: shifts up to 31 bits at a time

### Features not yet functional

1. Division/remainder
1. Read from standard input in base 2, 8, 10 and 16
1. Print to standard output in base 2, 8, 10 and 16
1. Create string representation of number in base 2, 8, 10 and 16


## Getting started

Development has been aimed at and only tested on Linux (Ubuntu 19 and Mint 19.1 distros).

Everything can be built with the `make` utility.
