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

int __mythread_mutex_unlock(mutex_t *mutex);

int __mythread_mutex_lock(mutex_t *mutex);

#endif
