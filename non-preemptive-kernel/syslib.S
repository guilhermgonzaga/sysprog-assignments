
/*
   Created in Oct 15, 2009.
   This file replaces the original syslib.c.
   SYSCALL in syslib.c changed %eax and %eflags
   before calling kernel_entry. This bug is
   fixed here.
*/

.text
.code32
.EQU ENTRY_POINT, 0x0f00

#define SYSCALL(i) \
	pushl	%eax; \
	movl	ENTRY_POINT, %eax; \
	pushl	i;\
	call	*%eax;\
	popl	%eax; \
	popl	%eax

.globl yield
.globl exit

yield:
	SYSCALL($0)
	ret

exit:
	SYSCALL($1)
	ret
