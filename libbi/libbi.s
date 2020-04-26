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
	pushq	%r13
	pushq	%r12
	pushq	%rbp
	pushq	%rbx
	subq	$5656, %rsp
	movq	%rdi, %rbp	# store address of n
	movq	%rsi, %rbx	# store value of b

	xorq	%rax, %rax

	leaq	1536(%rsp), %rdi
	movq	$1024, %rcx
	rep	stosl
	movb	$0, (%rdi)	#initialize buffer to all zeros

	leaq	1024(%rsp), %rdi
	movq	$128, %rcx
	rep	stosl
	movl	%ebx, 1532(%rsp)	# initialize auxiliary BigInt to b

	leaq	512(%rsp), %rdi
	movq	$128, %rcx
	rep	stosl
	movl	$1, 1020(%rsp)	# initialize auxiliary BigInt to 1

	xorq	%rax, %rax	# syscall number for read
	xorq	%rdi, %rdi	# stdin file descriptor
	leaq	1536(%rsp), %rsi	# address of buffer
	movq	$4097, %rdx	# size of buffer
	syscall

	movzbl	1536(%rsp), %r13d
	cmpb	$45, %r13b
	sete	%r12b
	movq	%rbx, %rsi
	leaq	1536(%rsp), %rdi
	call	sanitizeInput
	testq	%rax, %rax
	je	.L20read	# input invalid if string is empty

	movzbl	%r12b, %r12d
	leaq	-1(%rax), %rbx

	.L1read:	# store translated input in n
	cmpq	%r12, %rbx
	jl	.L23read

	xorl	%eax, %eax
	movq	%rsp, %rdi
	movq	$128, %rcx
	rep	stosl
	movsbl	1536(%rsp, %rbx), %eax
	movl	%eax, 508(%rsp)	# initialize temporary BigInt to next digit

	movq	%rsp, %rdx	# set parameters
	leaq	512(%rsp), %rsi
	movq	%rsp, %rdi
	call	BigIntMul	# get positional value of digit

	movq	%rbp, %rdx	# set parameters
	movq	%rsp, %rsi
	movq	%rbp, %rdi
	call	BigIntAdd	# add value of digit to n

	leaq	512(%rsp), %rdx
	leaq	1024(%rsp), %rsi
	movq	%rdx, %rdi
	call	BigIntMul	# update position

	decq	%rbx
	jmp	.L1read		# loop for all characters in string

	.L23read:
	cmpb	$45, %r13b
	je	.L24read
	movl	$1, %eax

	.L16read:	# end of function
	addq	$5656, %rsp
	popq	%rbx
	popq	%rbp
	popq	%r12
	popq	%r13
	ret

	.L24read:	# fix for negative input
	movq	%rbp, %rdi
	call	BigIntCompl	# get two's complement of n
	movl	$1, %eax	# valid input
	jmp	.L16read

	.L20read:	# fix for empty input
	movl	$0, %eax	# invalid input
	jmp	.L16read


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
	pushq	%r12
	pushq	%rbx
	subq	$1024, %rsp	# allocate 2 temporary BigInts
	movq	%rdi, %r8	# store address of n
	movq	%rsi, %rbx	# store address of buf
	movq	%rdx, %r12	# store value of b

	leaq	512(%rsp), %rdi
	movq	$128, %rcx
	movl	$0, %eax
	rep	stosl
	movl	%edx, 1020(%rsp)	# initialize temporary BigInt to b

	movq	%r8, %rsi	# set parameters
	movq	%rsp, %rdi
	callq	BigIntAssign	# Make a copy of n to work on

	cmpq	$10, %r12
	jne	.C1tostr

	cmpl	$0, (%rsp)
	js	.C2tostr

	.C1tostr:	# in case b != 10
	leaq	512(%rsp), %rdx	# address of BigInt containing b
	movq	%rbx, %rsi	# addrss of buf
	movq	%rsp, %rdi	# address of temporary BigInt
	callq	bi2all		# create string representation for n
	movq	%rax, %r12	# offset of the string in buf

	.C4tostr:
	movq	$4096, %rdx
	subq	%r12, %rdx	# length of string
	leaq	(%rbx, %r12), %rsi	# start of string in buf
	movq	%rbx, %rdi	# address of buffer
	xorq	%rcx, %rcx

	.L1tostr:	# move string to start of buf
	movb	(%rsi), %cl
	movb	%cl, (%rdi)
	incq	%rsi		# next character address
	incq	%rdi		# next character address
	decq	%rdx
	cmpq	$0, %rdx
	jg	.L1tostr	# loop for all characters in string

	movq	$4096, %rdi
	subq	%r12, %rdi	# length of string
	cmpq	$0, %rdi
	jle	.C3tostr	# loop only if string is not empty

	addq	%rbx, %rdi	# end address of string in buf
	movq	%rbx, %rax	# address of buf

	.L2tostr:	# convert from digits to ASCII
	movzbl	(%rax), %ecx	# get character from string
	leal	48(%ecx), %esi	# character + '0'
	leal	55(%ecx), %edx	# character + 'A' - 10
	cmpb	$9, %cl
	cmovle	%esi, %edx	# <- (character <= 9) ? (0 ~ 9) : (A ~ F)
	movb	%dl, (%rax)	# update character in string

	incq	%rax		# next character address
	cmpq	%rax, %rdi
	jg	.L2tostr	# until end of string
	movb	$0, (%rdi)	# add terminating null character

	.C3tostr:	# end of function
	movq	%rbx, %rax	# return address of buf
	addq	$1024, %rsp
	popq	%rbx
	popq	%r12
	ret

	.C2tostr:	# in case n is negative
	movq	%rsp, %rdi	# address of temporary BigInt
	callq	BigIntCompl	# get 2's complement for temporary

	leaq	512(%rsp), %rdx	# address of BigInt containing b
	movq	%rbx, %rsi	# addrss of buf
	movq	%rsp, %rdi	# address of temporary BigInt
	callq	bi2all		# create string representation for n

	leaq	-1(%rax), %r12	# offset of the string in buf
	movb	$-3, -1(%rbx, %rax)

	jmp	.C4tostr


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
        subq    $8, %rsp

        movq    %rdi, %rax
        movq    %rsi, %rdi
        movq    %rax, %rsi
        call    BigIntGT

        addq    $8, %rsp
        ret

// BigIntGT: returns x > y
// int BigIntGT(BigInt x, BigInt y);
BigIntGT:
        pushq   %rbx
        subq    $1024, %rsp
        movq    %rsi, %rbx
        movq    %rdi, %rsi
        leaq    512(%rsp), %rdi
        movl    $0, %eax
        call    BigIntAssign
        movq    %rbx, %rsi
        movq    %rsp, %rdi
        movl    $0, %eax
        call    BigIntAssign
        movl    512(%rsp), %eax
        testl   %eax, %eax
        js      .L2gt
        cmpl    $0, (%rsp)
        js      .L7gt
.L2gt:
        testl   %eax, %eax
        js      .L12gt
        movl    $0, %eax
.L5gt:
        cmpq    $127, %rax
        jg      .L13gt
        movl    (%rsp,%rax,4), %edx
        cmpl    %edx, 512(%rsp,%rax,4)
        ja      .L10gt
        addq    $1, %rax
        jmp     .L5gt
.L12gt:
        cmpl    $0, (%rsp)
        jns     .L9gt
        movl    $0, %eax
        jmp     .L5gt
.L13gt:
        movl    $0, %eax
        jmp     .L1gt
.L7gt:
        movl    $1, %eax
        jmp     .L1gt
.L9gt:
        movl    $0, %eax
        jmp     .L1gt
.L10gt:
        movl    $1, %eax
.L1gt:
        addq    $1024, %rsp
        popq    %rbx
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
	xorq	%r8, %r8
	movq	$127, %r10		# index

	.L1add:
	movl	(%rdi, %r10, 4), %ecx
	movl	(%rsi, %r10, 4), %eax

	addl	%r8d, %ecx		# sum carry in
	setc	%r8b			# set carry out
	addl	%eax, %ecx		# add numbers
	jnc	.C1add
	movb	$1, %r8b		# set carry out

	.C1add:
	movl	%ecx, (%rdx, %r10, 4)	# store result

	decq	%r10
	cmpq	$0, %r10
	jge	.L1add

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

	movq	%rsp, %rdi
	call	BigIntCompl	# get 2's complement for temporary (-y)

	movq	%r12, %rdx	# set parameters
	movq	%rsp, %rsi
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
        pushq   %rbp
        pushq   %rbx
        subq    $1544, %rsp
        movq    %rdi, %r8
        movq    %rsi, %rbp
        movq    %rdx, %rbx
        leaq    1024(%rsp), %rdi
        movl    $64, %ecx
        movl    $0, %eax
        rep stosq
        movq    %r8, %rsi
        leaq    512(%rsp), %rdi
        call    BigIntAssign
        movq    %rbp, %rsi
        movq    %rsp, %rdi
        movl    $0, %eax
        call    BigIntAssign
        leaq    1024(%rsp), %rsi
        movq    %rbx, %rdi
        movl    $0, %eax
        call    BigIntAssign
        cmpl    $0, (%rsp)
        js      .L10mul
        movl    $0, %ebp
.L2mul:
        cmpl    $0, 512(%rsp)
        jns     .L5mul
        xorq    $1, %rbp
        leaq    512(%rsp), %rdi
        movl    $0, %eax
        call    BigIntCompl
        jmp     .L5mul
.L10mul:
        movq    %rsp, %rdi
        movl    $0, %eax
        call    BigIntCompl
        movl    $1, %ebp
        jmp     .L2mul
.L12mul:
        movq    %rbx, %rdx
        leaq    512(%rsp), %rsi
        movq    %rbx, %rdi
        movl    $0, %eax
        call    BigIntAdd
.L4mul:
        movl    $1, %esi
        movq    %rsp, %rdi
        movl    $0, %eax
        call    BigIntShar
        movl    $1, %esi
        leaq    512(%rsp), %rdi
        movl    $0, %eax
        call    BigIntShl
.L5mul:
        leaq    1024(%rsp), %rsi
        movq    %rsp, %rdi
        movl    $0, %eax
        call    BigIntEq
        testl   %eax, %eax
        jne     .L11mul
        testb   $1, 508(%rsp)
        je      .L4mul
        jmp     .L12mul
.L11mul:
        testq   %rbp, %rbp
        jne     .L13mul
.L1mul:
        addq    $1544, %rsp
        popq    %rbx
        popq    %rbp
        ret
.L13mul:
        movq    %rbx, %rdi
        movl    $0, %eax
        call    BigIntCompl
        jmp     .L1mul

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
	movq	$127, %r10				# index
	xorq	%r8, %r8				# aux for carry of shift

	.L1shl:
	movl	(%rdi, %r10, 4), %ebx
	movl	%ebx, %edx 				# aux of %ebx
	movl	%esi, %ecx

	shl 	%cl, %ebx
	rol 	%cl, %edx		
	sub		%ebx, %edx				# calculates carry bits

	add		%r8d, %ebx				# sum with carry
	movl 	%ebx, (%rdi, %r10, 4)	
	movl	%edx, %r8d				# Set carry in r8d

	decq	%r10
	cmpq	$0, %r10
	jg	.L1shl
	
	ret


// BigIntShar: x = x >> n
// void BigIntShar(BigInt x, int n);
BigIntShar:
	xorq	%r10, %r10
	xorq	%r8, %r8				# aux for carry of shift

	.L1shr:
	movl 	(%rdi, %r10, 4), %ebx 
	movl	%ebx, %edx 				# aux of %ebx
	movl	%esi, %ecx

	shr 	%cl, %ebx
	ror 	%cl, %edx				
	sub		%ebx, %edx				# calculates carry bits

	add		%r8d, %ebx				# sum with carry
	movl 	%ebx, (%rdi, %r10, 4)	
	movl	%edx, %r8d				# Set carry in r8d

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
	movq	$128, %rcx
	movl	$0, %eax
	rep	stosl
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

// bi2all: convert BigInt to BigIntStr in base 2, 8, 10 or 16.
// Destroys input num.
// int bi2all(BigInt num, BigIntStr buffer, BigInt base);
bi2all:
	pushq	%r13
	pushq	%r12
	pushq	%rbp
	pushq	%rbx
	subq	$1032, %rsp	# space for 2 BigInts

	movq	%rdi, %rbx	# store address of num
	movq	%rdx, %r13	# store address of base

	movq	%rsp, %rdi	# address of the BigInts
	movq	$256, %rcx	# 128 quadwords
	movl	$0, %eax
	rep	stosl		# initialize 2 BigInts to zero

	// movq	%rsi, %rdi	# address of buffer
	// movq	$1024, %rcx
	// movl	$0, %eax
	// rep	stosl		# initialize buffer to zero

	leaq	4095(%rsi), %rbp
	movq	$4095, %r12	# end index of buffer
	// movq	$0, 1(%rbp)	# add terminating null character

	.L1bi2all:	# fill buffer with digits
	leaq	512(%rsp), %rsi	# set parameters
	movq	%rbx, %rdi
	callq	BigIntEq	# compare num to zero
	testq	%rax, %rax
	jne	.C1bi2all	# loop while num != 0

	movq	%rsp, %rdx	# set parameters
	movq	%r13, %rsi
	movq	%rbx, %rdi
	callq	BigIntMod	# temporary <- next digit

	movl	508(%rsp), %eax	# next digit
	movb	%al, (%rbp)	# buffer <- next digit

	movq	%rbx, %rdx	# set parameters
	movq	%r13, %rsi
	movq	%rbx, %rdi
	callq	BigIntMul	# num <- num * base

	decq	%r12
	decq	%rbp
	cmpq	$0, %r12
	jge	.L1bi2all	# loop for all digits in base

	addq	$1, %r12	# Fix to index on the first character

	.C1bi2all:	# end of function
	movq	%r12, %rax	# start index of the string in buffer
	addq	$1032, %rsp
	popq	%rbx
	popq	%rbp
	popq	%r12
	popq	%r13
	ret


// sanitizeInput: verify input and make it in the range [0, base).
// Returns the index one past the end of the string.
// int sanitizeInput(BigIntStr buffer, int base);
sanitizeInput:
	cmpq	$10, %rsi
	je	.C2sanInput	# check negative input for base 10 only

	movq	%rdi, %rax	# iterator to buffer
	.C1sanInput:	# verify empty input
	cmpb	$0, (%rax)
	jne	.L4sanInput
	movl	$0, %eax	# invalid input
	ret

	.C2sanInput:	# base 10
	cmpb	$45, (%rdi)
	je	.C3sanInput	# jump if first character is '-'
	movq	%rdi, %rax	# iterator to buffer
	jmp	.C1sanInput

	.C3sanInput:	# negative input
	leaq	1(%rdi), %rax	# fix for leading minus sign
	jmp	.C1sanInput

	.L6sanInput:
	leal	-97(%rdx), %ecx
	cmpb	$5, %cl
	ja	.L5sanInput
	subl	$39, %edx
	movb	%dl, (%rax)

	.L5sanInput:
	movzbl	(%rax), %ecx
	leal	-48(%rcx), %edx
	movb	%dl, (%rax)
	testb	%dl, %dl
	js	.C4sanInput	# end function if character < 0
	movsbq	%dl, %rdx
	cmpq	%rsi, %rdx
	jge	.C4sanInput	# end function if character >= base
	incq	%rax

	.L4sanInput:
	leaq	4096(%rdi), %rdx
	cmpq	%rax, %rdx
	jbe	.C5sanInput
	movzbl	(%rax), %edx
	testb	%dl, %dl
	je	.C5sanInput
	cmpq	$16, %rsi
	jne	.L5sanInput

	leal	-65(%rdx), %ecx
	cmpb	$5, %cl
	ja	.L6sanInput
	subl	$7, %edx
	movb	%dl, (%rax)
	jmp	.L5sanInput

	.C5sanInput:	# end of function
	subq	%rdi, %rax	# index one past the end of string
	ret

	.C4sanInput:	# fix for stray characters
	movl	$0, %eax
	ret
