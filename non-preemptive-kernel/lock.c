/* lock.c: mutual exclusion
 * If SPIN is undefined, lock_acquire() should block the calling process until its request can be satisfied
 * Your solution must satisfy the FIFO fairness property
 */

#include "common.h"
#include "scheduler.h"
#include "lock.h"

#define SPIN


void lock_init(lock_t *lock)
{
#ifdef SPIN
    lock->status = UNLOCKED;
    lock->holderPid = -1;  // invalid pid
#else
    lock->status = UNLOCKED;
    lock->holderPid = -1;  // invalid pid
#endif
}

void lock_acquire(lock_t *lock)
{
#ifdef SPIN
    while (lock->status == LOCKED) {
        do_yield();
    }
    lock->status = LOCKED;
#else
    if (lock->status == UNLOCKED) {
        lock->status = LOCKED;
        lock->holderPid = current_running->pid;
    }
    else if (current_running->pid != lock->holderPid) {  // Check for double acquire
        block(lock);
        do_yield();
    }
#endif
}

void lock_release(lock_t *lock)
{
#ifdef SPIN
    lock->status = UNLOCKED;
    lock->holderPid = -1;  // invalid pid
#else
    if (lock->status == LOCKED) {
        if (blocked_tasks(lock)) {
            unblock(lock);
            // holderPid set in unblock
        }
        else {
            lock->status = UNLOCKED;
            lock->holderPid = -1;  // invalid pid
        }
    }
#endif
}
