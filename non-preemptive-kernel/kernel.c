/* kernel.c */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"

#include "tasks.c"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25


/* Insert a PCB object in the back of the queue */
extern void enqueue(pcb_queue_t *queue, pcb_t *pcb);

/* Remove and return the first PCB object in the queue */
extern pcb_t *dequeue(pcb_queue_t *queue);

/* Inform that no tasks remain and never return */
void end_of_execution(void);


pcb_t *current_running = NULL;

static pcb_t pcbs[NUM_TASKS];


/* This function is the entry point for the kernel
 * It must be the first function in the file
 */
void _start(void)
{
    /* Set up the single entry-point for system calls */
    *ENTRY_POINT = &kernel_entry;

    clear_screen(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Initialize the pcbs and the ready queue */
    for (int i = 0; i < NUM_TASKS; i++) {
        pcbs[i].pid = i;
        pcbs[i].stack = STACK_MIN + (i + 1) * STACK_SIZE - 1;

        /* -4 gives space for the return address at first context switch */
        pcbs[i].reg.esp = pcbs[i].stack - 4;
        *(uint32_t *)pcbs[i].reg.esp = task[i]->entry_point;

        enqueue(&ready_queue, &pcbs[i]);
    }

    /* Make sure stacks are allocated in reserved space */
    ASSERT(pcbs[0].stack > STACK_MIN);
    ASSERT(pcbs[NUM_TASKS-1].stack <= STACK_MAX);

    /* Schedule the first task */
    scheduler_count = 0;
    scheduler_entry();

    /* We shouldn't ever get here */
    ASSERT(0);
}


void end_of_execution(void)
{
    char *message = "All tasks finished";
    int i = 0, j = strlen(message);

    print_str(i, 0, message);

    do {
        delay(8);
        print_char(i, j, '.');

        j = (j + 1) % SCREEN_WIDTH;
        if (j == 0) {
            i = (i + 1) % SCREEN_HEIGHT;
        }
    } while (i != 0 || j != 0);

    while (1) {
        // empty
    }
}
