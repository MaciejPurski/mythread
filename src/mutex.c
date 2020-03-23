#include <signal.h>
#include <stdio.h>
#include "mutex.h"


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

    mutex->private = (void *) _mutex;

    return 0;
}

void _mutex_lock(mutex_t *mutex)
{
    mutex_internal *_mutex = (mutex_internal *) mutex->private;

    if (_mutex->taken) {
        thread_t *current = get_current_thread();

        printf("Block current thread\n");
        _mutex->n_waiting++;

        /* Put the current thread on the end of the
         * waiting queue */
        if (!_mutex->wait_queue) {
            _mutex->wait_queue = current;
        } else {
            thread_t *tmp = _mutex->wait_queue;
            while (tmp->next)
                tmp = tmp->next;

            tmp->next = current;
        }

        thread_wait(current);
    } else {
        _mutex->taken = true;
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

    _mutex->taken = false;

    if (_mutex->wait_queue) {
        thread_t *next = _mutex->wait_queue->next;
        // TODO: better wakeup all the threads waiting on the mutex
        thread_wakeup(_mutex->wait_queue);
        _mutex->wait_queue = next;
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