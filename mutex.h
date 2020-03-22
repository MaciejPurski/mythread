#ifndef C_THREADS_MUTEX_H
#define C_THREADS_MUTEX_H

#include <stdbool.h>
#include "thread.h"

typedef struct mutex_t {
    bool taken;
    thread_t *wait_queue;
    unsigned int n_waiting;
} mutex_t;


int __mythread_mutex_unlock(mutex_t *mutex);

int __mythread_mutex_lock(mutex_t *mutex);

#endif
