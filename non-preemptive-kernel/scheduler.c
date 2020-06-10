/* scheduler.c */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "util.h"


int scheduler_count;

/* Declare queues here */
pcb_queue_t ready_queue;


/* Insert a PCB object in the back of the queue */
void enqueue(pcb_queue_t *queue, pcb_t *pcb);

/* Remove and return the first PCB object in the queue */
pcb_t *dequeue(pcb_queue_t *queue);


void scheduler(void)
{
    if (current_running != NULL) {
        enqueue(&ready_queue, current_running);
    }
    current_running = dequeue(&ready_queue);

    ++scheduler_count;
}

void do_yield(void)
{
    scheduler_entry();
}

void do_exit(void)
{
    current_running = NULL;
    scheduler_entry();
}

void block(lock_t *lock)
{
    enqueue(&lock->blocked_tasks, current_running);
    current_running = NULL;
}

void unblock(lock_t *lock)
{
    pcb_t *temp = dequeue(&lock->blocked_tasks);
    lock->holderPid = temp->pid;
    enqueue(&ready_queue, temp);
}

_Bool blocked_tasks(lock_t *lock)
{
    return lock->blocked_tasks.begin != NULL;
}

void enqueue(pcb_queue_t *queue, pcb_t *pcb)
{
    pcb->next = NULL;

    if (queue->end != NULL)  // if queue not empty
        queue->end->next = pcb;
    else
        queue->begin = pcb;

    queue->end = pcb;
}

pcb_t *dequeue(pcb_queue_t *queue)
{
    pcb_t *first = NULL;

    if (queue->begin != NULL) {  // if queue not empty
        first = queue->begin;
        queue->begin = queue->begin->next;
        if (queue->begin == NULL)  // last element just removed
            queue->end = NULL;
        else
            first->next = NULL;
    }

    return first;
}
