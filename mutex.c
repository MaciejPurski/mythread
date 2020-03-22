#include <signal.h>
#include <stdio.h>
#include "mutex.h"


int mythread_mutex(mutex_t *mutex)
{
    mutex->taken = false;
    mutex->n_waiting = 0;
    mutex->wait_queue = NULL;

    // TODO: return something more interesting
    return 0;
}

int __mythread_mutex_lock(mutex_t *mutex)
{
    if (mutex->taken) {
        thread_t *current = get_current_thread();

        printf("Block current thread\n");
        mutex->n_waiting++;

        if (!mutex->wait_queue) {
            mutex->wait_queue = current;
        } else {
            thread_t *tmp = mutex->wait_queue;
            while (tmp->next)
                tmp = tmp->next;

            tmp->next = current;
        }

        thread_wait(current);
    } else {
        mutex->taken = true;
    }
}

int mythread_lock(mutex_t *mutex)
{
    CRIT_SECTION_BEGIN();
    __mythread_mutex_lock(mutex);
    CRIT_SECTION_END();
}

int __mythread_mutex_unlock(mutex_t *mutex)
{
    mutex->taken = false;

    if (mutex->wait_queue) {
        thread_t *next = mutex->wait_queue->next;
        // TODO: better wakeup all the threads waiting on the mutex
        thread_wakeup(mutex->wait_queue);
        mutex->wait_queue = next;
    }
}

int mythread_unlock(mutex_t *mutex)
{
    CRIT_SECTION_BEGIN();
    __mythread_mutex_unlock(mutex);
    CRIT_SECTION_END();
}