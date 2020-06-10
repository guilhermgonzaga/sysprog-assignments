/* scheduler.h */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "kernel.h"
#include "lock.h"


/* The queue of tasks waiting to be scheduled */
extern pcb_queue_t ready_queue;

/* Number of times scheduler() was called */
extern int scheduler_count;

/* Save the context and the kernel stack before calling scheduler
 * This function is implemented in entry.S
 */
void scheduler_entry(void);

/* Change current_running to the next task */
void scheduler(void);

/* Schedule another task
 * Call from a kernel thread or kernel_entry()
 */
void do_yield(void);

/* Schedule another task
 * Do not reschedule the current one
 * Call from a kernel thread or kernel_entry()
 */
void do_exit(void);

/* Put the currently running task on the blocked queue */
void block(lock_t *lock);

/* If there is a task on the blocked queue, move it to the ready queue */
void unblock(lock_t *lock);

/* Returns true if there are blocked tasks, false otherwise */
_Bool blocked_tasks(lock_t *lock);


#endif                          /* SCHEDULER_H */
