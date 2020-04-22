// Use tab 8 for better formating

	.text
	.globl BigIntRead, BigIntPrint, BigIntToStr
	.globl BigIntEq, BigIntLT, BigIntGT
	.globl BigIntAssign, BigIntAdd, BigIntSub
	.globl BigIntMul, BigIntDiv, BigIntMod
	.globl BigIntAnd, BigIntOr, BigIntXor
	.globl BigIntShl, BigIntShar, BigIntNeg
	.globl BigIntCompl

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
	subq	$4120, %rsp	# reserve space for buffer

	movq	%rsi, %rdx
	movq	%rsp, %rsi	# address of the buffer
	call	BigIntToStr

	movq	$1, %rax	# write syscall number
	movq	$1, %rdi	# write file descriptor
	movq	%rsp, %rsi	# address of the buffer
	movq	$4096, %rdx	# size of the buffer
	syscall

	addq	$4120, %rsp
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
	xorq	%r10, %r10

	movl (%rdi), %ecx 
	testl	%ecx, %ecx
	je .L1ltneg
	movl	(%rsi), %edx
	testl	%ecx, %edx
	je .L1ltneg

	.L1lt:
	movl    (%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %edx
	cmpl	%ecx, %edx
	jg	.C1lt

	incq	%r10
	cmpq	$128, %r10
	jl	.L1lt

	movq	$1, %rax
	jg	.C1lt

	.L1ltneg:
	movl    (%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %edx
	cmpl	%ecx, %edx
	jl	.C1lt

	incq	%r10
	cmpq	$128, %r10
	jl	.L1ltneg

	movq	$1, %rax

	.C1lt:
	ret

// BigIntGT: returns x > y
// int BigIntGT(BigInt x, BigInt y);
BigIntGT:
	xorq	%rax,%rax
	xorq	%r10, %r10

	movl	(%rdi), %ecx
	testl	%ecx, %ecx
	je .L1lgneg
	movl	(%rsi), %edx
	testl	%edx, %edx
	je .L1lgneg

	.L1gt:
	movl    (%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %edx
	cmpl	%ecx, %edx
	jl	.C1gt

	incq	%r10
	cmpq	$128, %r10
	jl	.L1gt

	.L1lgneg:
	movl    (%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %edx
	cmpl	%ecx, %edx
	jl	.C1gt

	incq	%r10
	cmpq	$128, %r10
	jl	.L1lgneg

	movq	$1, %rax

	movq	$1, %rax
	.C1gt:
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
	movq	$128, %r10

	.L1add:
	movl	(%rdi, %r10, 4), %ecx
	addb 	%bl, %cl				# sum carry
	movl	(%rsi, %r10, 4), %eax

	addl	%eax, %ecx	
	cmpl	$15, %ecx				# if set carry out
	setg 	%bl 
	andl	$15, %ecx				# get 4 bits of sum
	movl	%ecx, (%rdx, %r10, 4)

	decq	%r10
	cmpq	$0, %r10
	jg	.L1add

	addb 	%bl, %cl
	
	ret

// BigIntSub: xmy = x - y
// void BigIntSub(BigInt x, BigInt y, BigInt xmy);
BigIntSub:
	pushq	%r12
	pushq	%rbx
	pushq	%rbp
	movq	%rdi, %rbp	# store address of x
	movq	%rsi, %rbx	# store address of y
	movq	%rdx, %r12	# store address of xmy

	subq	$512, %rsp	# allocate temporary BigInt on the stack
	movq	%rsp, %rdi	# set parameter
	call	BigIntAssign	# initialize temporary BigInt to y

	movq	%rsi, %rdi
	call	BigIntCompl	# get 2's complement for temporary (-y)

	movq	%r12, %rdx	# set parameters
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	call	BigIntAdd	# x += -y

	addq	$512, %rsp
	popq	%rbp
	popq	%rbx
	popq	%r12
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
	xorq	%r10, %r10

	.L1xor:
	movl	(%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %eax
	xorl	%eax, %ecx
	movl	%ecx, (%rdx, %r10, 4)

	incq	%r10
	cmpq	$128, %r10
	jl	.L1xor
	ret

// BigIntShl: x = x << n
// void BigIntShl(BigInt x, int n);
BigIntShl:
	xorq	%r10, %r10

	.L1shl:
	movl (%rdi, %r10, 4), %ebx 
	add %ecx, %ebx			# sum with the last shift bits
	movl %esi, %ecx 

	sal %cl, %ebx
	mov %ebx, %ecx

	andl $240, %ebx			# set zero, ebx &= 0xf0.
	movl %ebx, (%rdi, %r10, 4)	

	sar $4, %ecx			# get rest of bits 

	incq	%r10
	cmpq	$128, %r10
	jl	.L1shl

	ret

// BigIntShar: x = x >> n
// void BigIntShar(BigInt x, int n);
BigIntShar:
	xorq	%rax,%rax
	xor		%ecx, %ecx

	.L1shr:
	movl (%rdi, %r10, 4), %ebx 
	movl %esi, %ecx 
	sar %cl, %ebx

	movl %ebx, (%rdi, %r10, 4)	
	
	incq	%r10
	cmpq	$128, %r10
	jl	.L1shr

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


// BigIntCompl: x = -x
// void BigIntCompl(BigInt x);
BigIntCompl:
	pushq	%rbx
	movq	%rdi, %rbx	# store address of x

	subq	$512, %rsp	# allocate temporary BigInt on the stack
	movq	%rsp, %rdi	# set address of temporary BigInt
	movl	$64, %ecx
	movl	$0, %eax
	rep	stosq
	movl	$1, 508(%rsp)	# initialize temporary BigInt to 1

	movq	%rbx, %rdi	# set parameter
	call	BigIntNeg

	movq	%rbx, %rdx	# set parameters
	movq	%rsp, %rsi
	movq	%rbx, %rdi
	call	BigIntAdd

	addq	$512, %rsp
	popq	%rbx
	ret
