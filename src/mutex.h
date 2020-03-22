#ifndef C_THREADS_MUTEX_H
#define C_THREADS_MUTEX_H

#include <stdbool.h>
#include <mythread/types.h>


int __mythread_mutex_unlock(mutex_t *mutex);

int __mythread_mutex_lock(mutex_t *mutex);

#endif
