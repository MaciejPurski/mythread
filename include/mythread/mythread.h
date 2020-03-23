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

/**
 * mythread_start() is used to create a new thread. It doesn't make a thread run.
 *
 * @param thread Pointer to a function which will be ran in a new thread. A function
 *          should finish its execution with a call to mythread_exit()
 * @param args Pointer to function arguments.
 * @param priority Number from 0 to 10 - the lesser number, the greater priority.
 * @return Returns 0 on success and -1 on failure.
 */
int mythread_start(void (*thread)(void *), void *args, unsigned int priority);

/**
 * A function, which should be called only from an executed thread. It destroys
 * a currently executed thread and changes context to a next thread from
 * a running queue. If there are no more threads in the queue, the function
 * causes return to the main thread and stops the scheduling mechanism.
 */
void mythread_exit(void);

/**
 * This function starts the scheduling mechanism by calling alarm() function
 * and passing execution to a first thread from the queue. It hangs the main
 * thread and returns when all threads created by mythread_start() are finished.
 */
void mythread_join(void);

/**
 * This function initializes a mutex_t struct.
 * @param mutex Pointer to mutex_t struct
 * @return 0 on success, -1 on failure
 */
int mythread_mutex(mutex_t *mutex);

/**
 * mythread_lock() tries to take the given mutex. If it is taken,
 * an executing thread is going to be suspended and moved away from
 * the ready threads queue. A context is changed to a next thread.
 * @param mutex Pointer to mutex_t struct. Mutex have been initialized
 *              by a call to mythread_mutex().
 * @return 0 on success, -1 on failure
 */
int mythread_lock(mutex_t *mutex);

/**
 * mythread_unlock() releases the given mutex. It causes all threads
 * suspended on the mutex to be woken up - they return to the ready
 * threads queue.
 * @param mutex Pointer to mutex_t struct. Mutex have been initialized
 *              by a call to mythread_mutex(). Mutex does not have to
 *              be previously locked.
 * @return 0 on success, -1 on failure
 */
int mythread_unlock(mutex_t *mutex);

int mythread_cond(cond_t *cond);

int mythread_wait(cond_t *cond, mutex_t *mutex);

int mythread_signal(cond_t *_cond);

int mythread_destroy(void *o);

#endif
