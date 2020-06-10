.data
format_string:
	.asciz	"value = %d\n"
ecx_save:
	.long	0

.code32
.text
# int timestwo(int n);
.globl	timestwo
timestwo:
	# Prologue
	pushl	%ebp
	movl	%esp, %ebp
	movl	%ecx, ecx_save
	# printf(format_string, value);
	movl	value, %ecx
	pushl	%ecx
	pushl	$format_string
	call	printf
	# return n+n;
	# (Return value goes in %eax)
	movl	8(%ebp), %ecx
	addl	%ecx, %ecx
	movl	%ecx, %eax
	# Epilogue
	movl	ecx_save, %ecx
	leave
	ret
