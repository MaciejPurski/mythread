#include "mythread.h"
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>
#include <ucontext.h>
#include <stdbool.h>

#define ARR_SIZE(x) sizeof(x) / sizeof(x[0])
#define BURN_ITERATIONS 100000000

typedef struct {
    char name[32];
    unsigned int priority;
    int n_iterations;
} thread_args;

struct mutex_t mutex;

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

void thread1() {
    printf("Thread 1 started\n");
    int counter = 0;
    while (counter < 10) {
        printf("a\n");
        for(int i = 0; i < 100000000; i++) {
        }
        counter++;
    }
}

void thread2() {
    printf("Thread 2 started\n");
    int counter = 0;
    while (counter < 10) {
        printf("b\n");
        for(int i = 0; i < 100000000; i++) {
        }
        counter++;
    }
}

void thread3() {
    printf("Thread 3 started\n");
    int counter = 0;
    while (counter < 10) {
        printf("c\n");
        for(int i = 0; i < 100000000; i++) {
        }
        counter++;
    }
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

    threads_join();

    printf("Threads exited\n");

    return 0;
}