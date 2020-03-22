#include <stdio.h>
#include "cond.h"

int mythread_cond(cond_t *cond)
{
    cond->mutex = NULL;
    cond->thread_waiting = NULL;
}

int mythread_wait(cond_t *cond, mutex_t *mutex)
{
    CRIT_SECTION_BEGIN();
    cond->mutex = mutex;

    if (!cond->mutex->taken) {
        perror("Mutex in cond variable must be taken!\n");
        return -1;
    }

    if (cond->thread_waiting) {
        perror("Only one thread can wait on the cond variable!\n");
        return -1;
    }

    __mythread_mutex_unlock(mutex);
    cond->thread_waiting = get_current_thread();
    thread_wait(cond->thread_waiting);

    __mythread_mutex_lock(mutex);
    CRIT_SECTION_END();
}

int mythread_signal(cond_t *cond)
{
    CRIT_SECTION_BEGIN();
    // TODO: maybe unnessecary?
    if (cond->mutex && !cond->mutex->taken) {
        perror("Mutex in cond variable must be taken!\n");
        return -1;
    }

    if (cond->thread_waiting) {
        thread_wakeup(cond->thread_waiting);
        cond->thread_waiting = NULL;
    }

    CRIT_SECTION_END();
}
