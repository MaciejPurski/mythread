#ifndef C_THREADS_COND_H
#define C_THREADS_COND_H

#include <mythread/mythread.h>
#include "thread.h"
#include "mutex.h"

typedef struct cond_struct {
    mutex_internal *mutex;
    thread_t *thread_waiting;
} cond_internal;

#endif
