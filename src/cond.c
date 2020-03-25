#include <stdio.h>
#include "cond.h"
#include "mutex.h"
#include "utils.h"

int mythread_cond(cond_t *cond)
{
    if (!cond) {
        perror("ERROR: cond is NULL!");
        return -1;
    }

    cond_internal *_cond = (cond_internal *) malloc(sizeof(cond_internal));

    if (!_cond) {
        perror("ERROR: Can't create cond variable, no memory!\n");
        return -1;
    }

    _cond->mutex = NULL;
    _cond->thread_waiting = NULL;

    cond->private = (void *) _cond;

    return 0;
}

int mythread_wait(cond_t *cond, mutex_t *mutex)
{
    if (!mutex || !cond) {
        perror("ERROR: cond and mutex must not be NULL!\n");
        return -1;
    }

    if (!mutex->private) {
        perror("ERROR: Mutex is not initialized!\n");
        return -1;
    }

    if (!cond->private) {
        perror("ERROR: Cond is not initialized!\n");
        return -1;
    }

    cond_internal *_cond = (cond_internal *) cond->private;
    mutex_internal *_mutex = (mutex_internal *) mutex->private;

    _cond->mutex = _mutex;

    if (!_cond->mutex->taken) {
        perror("Mutex in cond variable must be taken!\n");
        return -1;
    }

    if (_cond->mutex->thread_owning != get_current_thread()) {
        perror("Mutex must be owned by a calling thread!\n");
        return -1;
    }

    CRIT_SECTION_BEGIN();
    _mutex_unlock(mutex);
    thread_t *current = get_current_thread();

    thread_push(&_cond->thread_waiting, current);
    thread_wait(current);

    _mutex_lock(mutex);
    CRIT_SECTION_END();

    return 0;
}

int mythread_signal(cond_t *cond)
{
    if (!cond) {
        perror("ERROR: cond must not be NULL!\n");
        return -1;
    }

    if (!cond->private) {
        perror("ERROR: cond must be initialized!\n");
        return -1;
    }
    CRIT_SECTION_BEGIN();
    cond_internal *_cond = (cond_internal *) cond->private;

    thread_t *signaled_thread = thread_pop(&_cond->thread_waiting);

    if (signaled_thread)
        thread_wakeup(signaled_thread);

    CRIT_SECTION_END();

    return 0;
}
