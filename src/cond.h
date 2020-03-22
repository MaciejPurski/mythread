#ifndef C_THREADS_COND_H
#define C_THREADS_COND_H

#include <mythread/mythread.h>
#include "mutex.h"
#include "thread.h"

struct cond_struct {
    mutex_t *mutex;
    thread_t *thread_waiting;
};

#endif
