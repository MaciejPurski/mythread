#ifndef C_THREADS_TYPES_H
#define C_THREADS_TYPES_H

#include <stdbool.h>

typedef struct thread_t thread_t;

typedef struct mutex_t {
    bool taken;
    thread_t *wait_queue;
    unsigned int n_waiting;
} mutex_t;


typedef struct {
    mutex_t *mutex;
    thread_t *thread_waiting;
} cond_t;

#endif
