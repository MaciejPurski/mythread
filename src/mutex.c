#include <signal.h>
#include <stdio.h>
#include "mutex.h"
#include "utils.h"


int mythread_mutex(mutex_t *mutex)
{
    if (!mutex) {
        perror("ERROR: mutex is NULL!\n");
        return -1;
    }

    mutex_internal *_mutex = (mutex_internal *) malloc(sizeof(mutex_internal));
    if (!_mutex) {
        perror("ERROR: Can't create mutex, no memory!\n");
        return -1;
    }

    _mutex->taken = false;
    _mutex->n_waiting = 0;
    _mutex->wait_queue = NULL;
    _mutex->thread_owning = NULL;
    mutex->private = (void *) _mutex;

    return 0;
}

void _mutex_lock(mutex_t *mutex)
{
    mutex_internal *_mutex = (mutex_internal *) mutex->private;

    if (_mutex->taken) {
        thread_t *current = get_current_thread();

        _mutex->n_waiting++;
        /* Push the current thread on the mutex's
         * wait queue
         */
        thread_push(&_mutex->wait_queue, current);
        thread_wait(current);
    } else {
        _mutex->taken = true;
        _mutex->thread_owning = get_current_thread();
    }
}

int mythread_lock(mutex_t *mutex)
{
    if (!mutex) {
        perror("ERROR: mutex is NULL!\n");
        return -1;
    }

    if (!mutex->private) {
        perror("ERROR: Trying to lock an uninitialized mutex!\n");
        return -1;
    }

    CRIT_SECTION_BEGIN();
    _mutex_lock(mutex);
    CRIT_SECTION_END();

    return 0;
}

void _mutex_unlock(mutex_t *mutex)
{
    mutex_internal *_mutex = (mutex_internal *) mutex->private;

    /* Let a first thread from the priority queue acquire
     * the mutex */
    thread_t *waiting_thread = thread_pop(&_mutex->wait_queue);
    if (waiting_thread) {
        _mutex->thread_owning = waiting_thread;
        thread_wakeup(waiting_thread);
    } else {
        _mutex->taken = false;
        _mutex->thread_owning = NULL;
    }
}

int mythread_unlock(mutex_t *mutex)
{
    if (!mutex) {
        perror("ERROR: mutex is NULL!\n");
        return -1;
    }

    if (!mutex->private) {
        perror("ERROR: Trying to lock an uninitialized mutex!\n");
        return -1;
    }

    CRIT_SECTION_BEGIN();
    _mutex_unlock(mutex);
    CRIT_SECTION_END();

    return 0;
}