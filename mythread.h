#ifndef C_THREADS_MYTHREAD_H
#define C_THREADS_MYTHREAD_H

#include "cond.h"
#include "mutex.h"


int mythread_start(void (*thread)(void *), void *args, unsigned int priority);

void mythread_exit(void);

void threads_join();

int mythread_mutex(mutex_t *mutex);

int mythread_lock(mutex_t *mutex);

int mythread_unlock(mutex_t *mutex);

int mythread_cond(cond_t *cond);

int mythread_wait(cond_t *cond, mutex_t *mutex);

int mythread_signal(cond_t *cond);

int mythread_destroy(void *o);

#endif //C_THREADS_MYTHREAD_H
