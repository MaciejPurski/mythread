#ifndef C_THREADS_MYTHREAD_H
#define C_THREADS_MYTHREAD_H

/*
 * Structs representing mutex and conditional variable. All actual
 * struct members are hidden from a user of the library in
 * order to prevent them from manipulting them manually. Internal
 * struct definitions can be found in cond.h and mutex.h.
 */
typedef struct {
    void *private;
} mutex_t;

typedef struct {
    void *private;
} cond_t;

int mythread_start(void (*thread)(void *), void *args, unsigned int priority);

void mythread_exit(void);

void threads_join();

int mythread_mutex(mutex_t *mutex);

int mythread_lock(mutex_t *mutex);

int mythread_unlock(mutex_t *mutex);

int mythread_cond(cond_t *cond);

int mythread_wait(cond_t *cond, mutex_t *mutex);

int mythread_signal(cond_t *_cond);

int mythread_destroy(void *o);

#endif
