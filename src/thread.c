#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "thread.h"
#include "utils.h"


/* A pointer to a linked-list head of threads ready to be scheduled */
static thread_t *ready = NULL;

/* A pointer to a currently running thread */
static thread_t *current = NULL;

/* A special pointer to scheduler's thread */
static thread_t *sched_thread = NULL;

static unsigned int n_threads = 0; /* Threads counter */

static ucontext_t main_ctx; /* Context struct for the main thread */


void mythread_join()
{
    if (ready) {
        alarm(1);
        current = thread_pop(&ready);
        swapcontext(&main_ctx, &current->ctx);
    }
}

/*
 * This helper functions sets execution context to a thread,
 * which is set as current.
 */
static void run_current_thread()
{
    current->state = THREAD_RUNNING;
    /* Reschedule the alarm, giving a new thread a full time slice */
    alarm(SCHEDULE_INTERVAL);
    setcontext(&current->ctx);
}

static void thread_destroy()
{
    thread_t *old = current;

    current = thread_pop(&ready);

    free(old);
    n_threads--;

    /* If there are still threads to be executed,
     * run next thread. Otherwise, go back to the
     * scheduler
     */
    if (current)
        run_current_thread();
    else
        setcontext(&sched_thread->ctx);
}

void mythread_exit()
{
    CRIT_SECTION_BEGIN();
    thread_destroy();
    CRIT_SECTION_END();
}

static void schedule()
{
    thread_push(&ready, current);
    current = thread_pop(&ready);

    current->state = THREAD_RUNNING;
    alarm(SCHEDULE_INTERVAL);
    swapcontext(&sched_thread->ctx, &current->ctx);
}

static void run_scheduler()
{
    while (current)
        schedule();

    /* If there are no more threads,
     * return to main */
    free(sched_thread);
    setcontext(&main_ctx);
}

static void signal_handler()
{
    swapcontext(&current->ctx, &sched_thread->ctx);
}

/* Function used to find a free thread id */
static unsigned int allocate_t_id()
{
    if (!sched_thread)
        return 0;

    /* If there is only a scheduler's thread */
    if (!current)
        return 1;

    if (n_threads == MAX_THREADS) {
        perror("ERROR: Threads number exceeded!\n");
        return THREADS_LIMIT_EXC;
    }

    thread_t *tmp_thread = current;
    unsigned int max_id = 1;
    
    while (tmp_thread) {
        if (tmp_thread->t_id > max_id)
            max_id = tmp_thread->t_id;

        tmp_thread = tmp_thread->next;
    }

    return max_id + 1;
}

static thread_t *thread_struct_init(void (*thread)(void *),
                                    void *args, unsigned int priority)
{
    thread_t *new_thread = malloc(sizeof(*sched_thread));

    if (!new_thread) {
        perror("ERROR: Not enough memory to create a new thread!\n");
        return NULL;
    }

    new_thread->t_id = allocate_t_id();
    new_thread->priority = priority;

    /* Create a new context */
    if (getcontext(&new_thread->ctx) != 0) {
        perror("ERROR: Can't create a context!\n");
        return NULL;
    }


    /* If a thread finishes without calling mythread_exit(),
     * return to main context for safety.
     */
    new_thread->ctx.uc_link = &main_ctx;
    /* Set stack */
    new_thread->ctx.uc_stack.ss_sp = new_thread->stack;
    new_thread->ctx.uc_stack.ss_size = sizeof(new_thread->stack);

    new_thread->state = THREAD_READY;

    makecontext(&new_thread->ctx, (void (*)(void))thread, 1, args);
    n_threads++;

    return new_thread;
}

/* A function called only once, when mythread_start is called for a first time.
 * It saves main context, creates scheduler thread and sets the signal handler */
static int threads_init()
{
    /* Save main context */
    if (getcontext(&main_ctx) != 0) {
        perror("ERROR: Can't save current context!\n");
        return -1;
    }

    /* Create the scheduler thread */
    sched_thread = thread_struct_init(run_scheduler, NULL, 0);

    if (!sched_thread)
        return -1;

    /* Alarm signal should be blocked in the scheduler */
    sigemptyset(&sched_thread->ctx.uc_sigmask);
    sigaddset(&sched_thread->ctx.uc_sigmask, SIGALRM);

    /* Set signal handler */
    struct sigaction action;
    /* Block SIGALRM in signal handler */
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGALRM);

    action.sa_handler = signal_handler;
    /* Allow restarting of IO functions */
    action.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &action, NULL);

    return 0;
}


int mythread_start(void (*thread)(void *), void *args, unsigned int priority)
{
    if (!thread) {
        perror("ERROR: No function pointer provided!\n");
        return -1;
    }

    if (priority > MAX_PRIORITY) {
        perror("ERROR: Provided priority value is greater than MAX_PRIORITY\n");
        return -1;
    }

    /* On first call, initialize scheduling mechanism */
    if (!sched_thread)
        threads_init();

    thread_t *new_thread = thread_struct_init(thread, args, priority);

    if (!new_thread)
        return -1;

    thread_push(&ready, new_thread);

    return 0;
}

thread_t *get_current_thread()
{
    return current;
}


void thread_wait(thread_t *thread)
{
    thread->state = THREAD_WAITING;

    /* Get next thread */
    current = thread_pop(&ready);
    swapcontext(&thread->ctx, &current->ctx);
}

void thread_wakeup(thread_t *thread)
{
    thread->state = THREAD_READY;
    thread_push(&ready, thread);
}

/*
 * A generic structure used only by mythread_destroy()
 * function. It is necessary in order to make mythread_destroy()
 * "polymorphic" regarding mutex_t and cond_t structs.
 */
struct mythread_object {
    void *private;
};

int mythread_destroy(void *o)
{
    struct mythread_object *_o = (struct mythread_object *) o;
    free(_o->private);

    return 0;
}
