#include <mythread/mythread.h>
#include <stdio.h>

#define ARR_SIZE(x) sizeof(x) / sizeof(x[0])
#define BURN_ITERATIONS 100000000

typedef struct {
    char name[32];
    unsigned int priority;
    int n_iterations;
} thread_args;

mutex_t mutex;

void thread_fun(void *args) {
    thread_args *t_args = (thread_args *)args;
    printf("Thread %s started\n", t_args->name);

    mythread_lock(&mutex);

    for (int i = 0; i < t_args->n_iterations; i++) {
        printf("Running: %s priority: %d \n", t_args->name, t_args->priority);
        for (int j = 0; j < BURN_ITERATIONS; j++) {}
    }

    mythread_unlock(&mutex);

    printf("Thread %s exiting\n", t_args->name);
    mythread_exit();
}

int main()
{
    mythread_mutex(&mutex);
    thread_args threads[] = { {"a", 5, 10},
                         {"b", 5, 10},
                         {"c", 0, 10},
                         {"d", 1, 10},
                         {"e", 1, 10} };
    for (int i = 0; i < ARR_SIZE(threads); i++)
        mythread_start(thread_fun, (void *) &threads[i], threads[i].priority);

    mythread_join();
    mythread_destroy(&mutex);

    printf("Threads exited\n");

    return 0;
}