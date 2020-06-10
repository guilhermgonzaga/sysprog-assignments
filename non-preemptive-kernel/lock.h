/* lock.h: mutual exclusion in the style of Birrell */

#ifndef LOCK_H
#define LOCK_H


typedef struct {
    enum {
        UNLOCKED,
        LOCKED,
    } status;
    int holderPid;
    pcb_queue_t blocked_tasks;
} lock_t;


void lock_init(lock_t *);
void lock_acquire(lock_t *);
void lock_release(lock_t *);

#endif                          /* LOCK_H */
