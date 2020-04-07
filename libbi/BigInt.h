#ifndef _BIGINT_H

#define _BIGINT_H

#define BIGINT_LEN	128
#define BIGINT_STR_LEN	1236

typedef int BigInt[BIGINT_LEN];
typedef char BigIntStr[BIGINT_STR_LEN];

/* Read a BigInt n in the base b. */
/* The base can be 2, 8, 10 or 16. */
int BigIntRead(BigInt n, int b);

/* Print a BigInt n in the base b. */
/* The base can be 2, 8, 10 or 16. */
int BigIntPrint(BigInt n, int b);

/* Convert n to string in the base b and store */
/* it in buf. The base can be 2, 8, 10 or 16. */
char *BigIntToStr(BigInt n, char *buf, int b);

/* BigIntEq: returns x == y */
int BigIntEq(BigInt x, BigInt y);

/* BigIntLT: returns x < y */
int BigIntLT(BigInt x, BigInt y);

/* BigIntGT: returns x > y */
int BigIntGT(BigInt x, BigInt y);


/* BigIntAssign: x = y */
void BigIntAssign(BigInt x, BigInt y);

/* BigIntAdd: xpy = x + y */
void BigIntAdd(BigInt x, BigInt y, BigInt xpy);

/* BigIntSub: xmy = x - y */
void BigIntSub(BigInt x, BigInt y, BigInt xmy);

/* BigIntMul: xty = x * y */
void BigIntMul(BigInt x, BigInt y, BigInt xty);

/* BigIntDiv: xdy = x / y */
void BigIntDiv(BigInt x, BigInt y, BigInt xdy);

/* BigIntMod: xmy = x % y */
void BigIntMod(BigInt x, BigInt y, BigInt xmy);

/* BigIntAnd: xay = x & y */
void BigIntAnd(BigInt x, BigInt y, BigInt xay);

/* BigIntOr: xoy = x | y */
void BigIntOr(BigInt x, BigInt y, BigInt xoy);

/* BigIntXor: xxy = x | y */
void BigIntXor(BigInt x, BigInt y, BigInt xxy);

/* BigIntShl: x = x << n */
void BigIntShl(BigInt x, int n);

/* BigIntShar: x = x >> n */
void BigIntShar(BigInt x, int n);

/* BigIntNeg: x = ~x */
void BigIntNeg(BigInt x);

#endif /* !_BIGINT_H */
