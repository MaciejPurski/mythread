#ifndef C_THREADS_MUTEX_H
#define C_THREADS_MUTEX_H

#include <mythread/mythread.h>
#include <stdbool.h>
#include "thread.h"


typedef struct _mutex_internal {
    bool taken;
    thread_t *wait_queue;
    unsigned int n_waiting;
} mutex_internal;


/*
 * Internal functions used to unlock and lock a mutex.
 * They must be called under SIGALRM blocked.
 */
void _mutex_unlock(mutex_t *mutex);
void _mutex_lock(mutex_t *mutex);

#endif
