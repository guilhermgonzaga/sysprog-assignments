// Use tab 8 for better formating

	.text
	.globl BigIntRead, BigIntPrint, BigIntToStr
	.globl BigIntEq, BigIntLT, BigIntGT
	.globl BigIntAssign, BigIntAdd, BigIntSub
	.globl BigIntMul, BigIntDiv, BigIntMod
	.globl BigIntAnd, BigIntOr, BigIntXor
	.globl BigIntShl, BigIntShar, BigIntNeg

// Read a BigInt n in the base b.
// The base can be 2, 8, 10 or 16.
// int BigIntRead(BigInt n, int b);
// Must create a local buffer for the input string.
BigIntRead:
	pushq	%rbp
	subq	$1240,%rsp	# Allocate a buffer for 1236-char string
				# Stack must be 16-byte aligned
				# 1236+8 = 1244, but 1244 is not
				# multiple of 16. 1240+8=1248, which
				# is multiple of 16.

	movl	$0,%eax		# sys_read
	movl	$0,%edi		# Standard input
	leaq	(%rsp),%rsi	# Address of the local buffer
	movl	$1235,%edx	# Maximum length of the input string
	syscall

	# After reading the input string, you must check if
	# it is valid. For example, a base-2 number should have
	# only the digits [0-1], a base-10 number should have
	# only the digits 0-9, and so on.


	# deallocate the local variable and restore the stack
	# to where it was at the beginning of the function
	addq	$1240,%rsp
	popq	%rbp
	ret

// Print a BigInt n in the base b.
// The base can be 2, 8, 10 or 16.
// int BigIntPrint(BigInt n, int b);
BigIntPrint:
	ret

// Convert n to string in the base b.
// The base can be 2, 8, 10 or 16.
// char *BigIntToStr(BigInt n, char *buf, int b);
BigIntToStr:
	ret

// BigIntEq: returns x == y
// int BigIntEq(BigInt x, BigInt y);
BigIntEq:
	xorq	%rax, %rax
	xorq	%r10, %r10

	.L1eq:
	movl	(%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %edx
	cmpl	%ecx, %edx
	jne	.C1eq

	incq	%r10
	cmpq	$128, %r10
	jl	.L1eq

	movq	$1, %rax
	.C1eq:
	ret

// BigIntLT: returns x < y
// int BigIntLT(BigInt x, BigInt y);
BigIntLT:
	xorq	%rax,%rax
	ret

// BigIntGT: returns x > y
// int BigIntGT(BigInt x, BigInt y);
BigIntGT:
	xorq	%rax,%rax
	ret

// BigIntAssign: x = y
// void BigIntAssign(BigInt x, BigInt y);
BigIntAssign:
	xorq	%r10, %r10

	.L1assign:
	movl	(%rsi, %r10, 4), %ecx
	movl	%ecx, (%rdi, %r10, 4)

	incq	%r10
	cmpq	$128, %r10
	jl	.L1assign

	ret

// BigIntAdd: xpy = x + y
// void BigIntAdd(BigInt x, BigInt y, BigInt xpy);
BigIntAdd:
	ret

// BigIntSub: xmy = x - y
// void BigIntSub(BigInt x, BigInt y, BigInt xmy);
BigIntSub:
	ret

// BigIntMul: xty = x * y
// void BigIntMul(BigInt x, BigInt y, BigInt xty);
BigIntMul:
	ret

// BigIntDiv: xdy = x / y
// void BigIntDiv(BigInt x, BigInt y, BigInt xdy);
BigIntDiv:
	ret

// BigIntMod: xmy = x % y
// void BigIntMod(BigInt x, BigInt y, BigInt xmy);
BigIntMod:
	ret

// BigIntAnd: xay = x & y
// void BigIntAnd(BigInt x, BigInt y, BigInt xay);
BigIntAnd:
	xorq	%r10, %r10

	.L1and:
	movl	(%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %eax
	andl	%eax, %ecx
	movl	%ecx, (%rdx, %r10, 4)

	incq	%r10
	cmpq	$128, %r10
	jl	.L1and

	ret

// BigIntOr: xoy = x | y
// void BigIntOr(BigInt x, BigInt y, BigInt xoy);
BigIntOr:
	xorq	%r10, %r10

	.L1or:
	movl	(%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %eax
	orl	%eax, %ecx
	movl	%ecx, (%rdx, %r10, 4)

	incq	%r10
	cmpq	$128, %r10
	jl	.L1or

	ret

// BigIntXor: xxy = x ^ y
// void BigIntXor(BigInt x, BigInt y, BigInt xxy);
BigIntXor:
	ret

// BigIntShl: x = x << n
// void BigIntShl(BigInt x, int n);
BigIntShl:
	ret

// BigIntShar: x = x >> n
// void BigIntShar(BigInt x, int n);
BigIntShar:
	ret

// BigIntNeg: x = ~x
// void BigIntNeg(BigInt x);
BigIntNeg:
	xorq	%r10, %r10

	.L1neg:
	movl	(%rdi, %r10, 4), %ecx
	notl	%ecx
	movl	%ecx, (%rdi, %r10, 4)

	incq	%r10
	cmpq	$128, %r10
	jl	.L1neg

	ret
