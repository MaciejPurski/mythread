#ifndef C_THREADS_COND_H
#define C_THREADS_COND_H

#include "mutex.h"
#include "thread.h"

typedef struct {
    mutex_t *mutex;
    thread_t *thread_waiting;
} cond_t;

#endif