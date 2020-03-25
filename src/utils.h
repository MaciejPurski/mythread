#include "thread.h"

#ifndef C_THREADS_UTILS_H
#define C_THREADS_UTILS_H

/**
 * Helper method for thread prioirity queue management.
 * Used by thread.c, mutex.c and cond.c. head is a pointer
 * to a thread_t pointer, because push or pop might require
 * modification to a list's head.
 */
void thread_push(thread_t **head, thread_t *n_thread);
thread_t *thread_pop(thread_t **head);
#endif
