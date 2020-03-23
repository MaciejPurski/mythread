#include <stdio.h>
#include "cond.h"
#include "mutex.h"

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

    if (_cond->thread_waiting) {
        perror("Only one thread can wait on the cond variable!\n");
        return -1;
    }

    CRIT_SECTION_BEGIN();
    _mutex_unlock(mutex);
    _cond->thread_waiting = get_current_thread();
    thread_wait(_cond->thread_waiting);

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

    if (_cond->thread_waiting) {
        thread_wakeup(_cond->thread_waiting);
        _cond->thread_waiting = NULL;
    }

    CRIT_SECTION_END();

    return 0;
}
