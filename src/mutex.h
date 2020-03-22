#ifndef C_THREADS_MUTEX_H
#define C_THREADS_MUTEX_H

#include <mythread/mythread.h>
#include <stdbool.h>
#include "thread.h"


struct mutex_struct {
    bool taken;
    thread_t *wait_queue;
    unsigned int n_waiting;
};

int __mythread_mutex_unlock(mutex_t *mutex);

int __mythread_mutex_lock(mutex_t *mutex);

#endif
