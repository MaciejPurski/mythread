#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include "thread.h"

/* A pointer to a linked-list head - a currently executed thread*/
static thread_t *current = NULL;

static unsigned int n_threads = 0;

/* A special pointer to scheduler's thread */
static thread_t *sched_thread = NULL;

static ucontext_t main_ctx;

bool preempted = false;

// TODO: check errors everywhere
void threads_join()
{
    alarm(1);
    swapcontext(&main_ctx, &current->ctx);
}

// TODO: rename functions using __
void next_thread()
{
    current->state = THREAD_RUNNING;
    /* Reschedule the alarm, giving the new thread a full time slice */
    // TODO: maybe not reschedule?
    alarm(1);
    swapcontext(&sched_thread->ctx, &current->ctx);
}

void thread_destroy()
{
    thread_t *old = current;

    current = current->next;
    free(old);
    n_threads--;

    if (current) {
        next_thread();
    }
}

void mythread_exit()
{
    CRIT_SECTION_BEGIN();
    swapcontext(&current->ctx, &sched_thread->ctx);
    CRIT_SECTION_END();
}

void schedule()
{
    thread_t *old = current;
    thread_t *tmp = current;

    /* Perform round-robin among threads with same priority.
     * Find the last thread with the same priority as current */
    while (tmp->next && tmp->next->priority == old->priority) {
        tmp = tmp->next;
    }

    if (tmp != current) {
        current = current->next;
        old->next = tmp->next;
        tmp->next = old;
    }

    preempted = false;
    next_thread();
}

/* TODO: make functions static */
void run_scheduler()
{
    while (current) {
        /* If the scheduler was called without preemption
         * by a returning thread */
        if (!preempted) {
            thread_destroy();
        } else {
            schedule();
        }
    }

    /* If there are no more threads,
     * return to main */
    free(sched_thread);
    setcontext(&main_ctx);
}

void signal_handler()
{
    CRIT_SECTION_BEGIN();
    preempted = true;
    swapcontext(&current->ctx, &sched_thread->ctx);
    CRIT_SECTION_END();
}

/* Function used to find a free thread id */
unsigned int allocate_t_id()
{
    if (!sched_thread)
        return 0;

    /* If there is only a scheduler's thread */
    if (!current)
        return 1;

    if (n_threads == MAX_THREADS)
        return THREADS_LIMIT_EXC;


    thread_t *tmp_thread = current;
    unsigned int max_id = 1;

    /* Search for a first free id TODO: find lower values */
    while (tmp_thread) {
        if (tmp_thread->t_id > max_id)
            max_id = tmp_thread->t_id;

        tmp_thread = tmp_thread->next;
    }

    return max_id + 1;
}

thread_t *thread_struct_init(void (*thread)(void *), void *args, unsigned int priority)
{
    thread_t *new_thread = malloc(sizeof(*sched_thread));

    new_thread->t_id = allocate_t_id();
    new_thread->priority = priority;

    /* Create a new context */
    getcontext(&new_thread->ctx);
    if (!sched_thread)
        new_thread->ctx.uc_link = &main_ctx;
    else
        new_thread->ctx.uc_link = &sched_thread->ctx;
    // TODO: block signals in scheduler
    new_thread->ctx.uc_stack.ss_sp = new_thread->stack;
    new_thread->ctx.uc_stack.ss_size = sizeof(new_thread->stack);
    new_thread->state = THREAD_READY;

    makecontext(&new_thread->ctx, thread, 1, args);
    n_threads++;

    return new_thread;
}

/* A function called only once, when mythread_start is called for a first time.
 * It activates threading mechanisms and initializes scheduling */
void threads_init()
{
    /* Save main context */
    getcontext(&main_ctx);

    /* Create the scheduler thread */
    sched_thread = thread_struct_init(run_scheduler, NULL, 0);

    /* Alarm signal should be blocked in the scheduler */
    sigemptyset(&sched_thread->ctx.uc_sigmask);
    sigaddset(&sched_thread->ctx.uc_sigmask, SIGALRM);

    /* TODO: maybe use sigaction? */
    signal(SIGALRM, signal_handler);
}

/*
 * Add a thread to the priority queue
 */
void thread_add(thread_t *new_thread)
{
    if (!current) {
        current = new_thread;
    } else if (current->priority > new_thread->priority) {
        new_thread->next = current;
        current = new_thread;
    } else {
        thread_t *tmp = current;
        while (tmp->next && tmp->next->priority <= new_thread->priority) {
            tmp = tmp->next;
        }

        new_thread->next = tmp->next;
        tmp->next = new_thread;
    }
}

int mythread_start(void (*thread)(void *), void *args, unsigned int priority)
{
    // TODO: block signals, make THREAD_INIT a macro
    if (!sched_thread) {
        threads_init();
    }

    thread_t *new_thread = thread_struct_init(thread, args, priority);

    thread_add(new_thread);

    return new_thread->t_id;
}

thread_t *get_current_thread()
{
    return current;
}

void thread_wait(thread_t *thread)
{
    thread->state = THREAD_WAITING;

    /* Remove the current thread from running queue */
    current = current->next;
    thread->next = NULL;
    /* Go back to scheduler */
    // TODO: better way than a global variable?
    preempted = true;
    swapcontext(&thread->ctx, &sched_thread->ctx);
}

void thread_wakeup(thread_t *thread)
{
    thread->state = THREAD_READY;
    thread_add(thread);
}



