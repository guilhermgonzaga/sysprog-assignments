/* kernel.h: definitions used by kernel code */

#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"

/* ENTRY_POINT points to a location that holds a pointer to kernel_entry */
#define ENTRY_POINT ((void (**)(int)) 0x0f00)

/* System call numbers */
enum {
    SYSCALL_YIELD,
    SYSCALL_EXIT,
};

/* All stacks should be STACK_SIZE bytes large
 * The first stack should be placed at location STACK_MIN
 * Only memory below STACK_MAX should be used for stacks
 */
enum {
    STACK_MIN = 0x40000,
    STACK_SIZE = 0x1000,
    STACK_MAX = 0x52000,
};

struct regfile {
    int32_t eax;
    int32_t ebx;
    int32_t ecx;
    int32_t edx;
    int32_t esi;
    int32_t edi;
    int32_t ebp;
    int32_t esp;
    int32_t eflags;
};

typedef struct pcb {
    struct regfile reg;  // register bank backup
    int pid;
    uint32_t stack;  // stack start
    struct pcb *next;
} pcb_t;

typedef struct {
    pcb_t *begin;
    pcb_t *end;
} pcb_queue_t;


/* The task currently running */
extern pcb_t *current_running;

void kernel_entry(int fn);

#endif                          /* KERNEL_H */
