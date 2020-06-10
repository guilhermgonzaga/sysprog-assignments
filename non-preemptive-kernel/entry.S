// entry.S

/* Offset of register file in PCB */
#define	EAX	0
#define	EBX	4
#define	ECX	8
#define	EDX	12
#define	ESI	16
#define	EDI	20
#define	EBP	24
#define	ESP	28
#define	EFLAGS	32

	.data

	.text
	.code32

	.globl	kernel_entry
/* void kernel_entry(int fn);
 * Disambiguate yield and exit system calls.
 */
kernel_entry:
	cmpl	$0, 4(%esp)		// Check syscall number, 0 or 1
	jne	.ke_doexit		// if 1, call do_exit, else do_yield

	call	do_yield
	jmp	.ke_end

	.ke_doexit:
	call	do_exit

	.ke_end:
	ret


	.globl	scheduler_entry
/* void scheduler_entry(void);
 * Save register file state and call scheduler().
 */
scheduler_entry:
	/* Save context of current task if there is one */

	pushfl				// Push EFLAGS before it changes

	cmpl	$0, current_running	// Check for special case
	jne	.se_if

	subl	$4, %esp		// Restore stack
	jmp	.se_endif

	.se_if:
	pushl	%ebx			// Save original value
	movl	current_running, %ebx	// Address of register bank backup
	movl	%eax, EAX(%ebx)
	movl	%ebx, %eax		// Copy address of register bank backup
	popl	%ebx			// Restore original value

	popl	EFLAGS(%eax)

	movl	%ebx, EBX(%eax)
	movl	%ecx, ECX(%eax)
	movl	%edx, EDX(%eax)
	movl	%esi, ESI(%eax)
	movl	%edi, EDI(%eax)
	movl	%ebp, EBP(%eax)
	movl	%esp, ESP(%eax)

	.se_endif:

	/* Schedule another task */

	call	scheduler

	cmpl	$0, current_running	// Check for end of all tasks
	je	.se_endoftasks

	/* Restore context of scheduled task */

	movl	current_running, %eax	// Address of PCB of scheduled task

	movl	EBX(%eax), %ebx
	movl	ECX(%eax), %ecx
	movl	EDX(%eax), %edx
	movl	ESI(%eax), %esi
	movl	EDI(%eax), %edi
	movl	EBP(%eax), %ebp
	movl	ESP(%eax), %esp
	pushl	EFLAGS(%eax)		// Push value of EFLAGS
	popfl				// Restore EFLAGS after it won't change

	movl	EAX(%eax), %eax		// Restore EAX

	ret

	.se_endoftasks:
	call	end_of_execution	// Does not return
