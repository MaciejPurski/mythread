#ifndef C_THREADS_THREAD_H
#define C_THREADS_THREAD_H
#include <ucontext.h>
#include <stdlib.h>
#include <signal.h>

#define STACK_SIZE 8192
#define MAX_THREADS 64
#define THREADS_LIMIT_EXC 255 /* Special code informing that threads limit has been exceeded */
#define MAX_PRIORITY 100
#define DEFAULT_PRIORITY 50

typedef enum {
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_WAITING
} state_t;

#define CRIT_SECTION_BEGIN() sigset_t mask; \
        sigemptyset(&mask); \
        sigaddset(&mask, SIGALRM); \
        sigprocmask(SIG_BLOCK, &mask, NULL);\

#define CRIT_SECTION_END() \
    sigprocmask(SIG_UNBLOCK, &mask, NULL); \


typedef struct thread_t {
    unsigned int t_id; /* Thread id */
    unsigned int priority; /* Thread's execution priority, the lower value, the greater priority */
    ucontext_t ctx; /* Thread context structure */
    char stack[STACK_SIZE]; /* Buffer to keep context's stack */
    struct thread_t *next; /* Pointer to next linked-list element */
    state_t state;
} thread_t;

void thread_wait(thread_t *thread);
void thread_wakeup(thread_t *thread);
thread_t *get_current_thread();

#endif //C_THREADS_THREAD_H
