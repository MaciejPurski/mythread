#include <mythread/mythread.h>
#include <stdio.h>

#define TCOUNT 10
#define COUNT_LIMIT 5

int count = 0;
cond_t count_threshold_cv;
mutex_t count_mutex;

#define SLEEP() for (int k = 0; k < 400000000; k++) {}


void inc_count(void *arg){
    char *msg = (char *) arg;
    for (int i = 0; i < TCOUNT; i++) {
        mythread_lock(&count_mutex);
        count++;
        if (count == COUNT_LIMIT) {
            mythread_signal(&count_threshold_cv);
        }
        mythread_unlock(&count_mutex);
        printf("thread: %s count: %d \n", msg, count);
        SLEEP();
    }

    mythread_exit();
}

void watch_count(void *t)
{
    mythread_lock(&count_mutex);
    if (count < COUNT_LIMIT) {
        printf("Watch thread wait start!\n");
        mythread_wait(&count_threshold_cv,  &count_mutex);
        printf("Watch thread woken up!\n");
        count += 100;
    }
    mythread_unlock(&count_mutex);
    mythread_exit();
}

int main()
{
    mythread_mutex(&count_mutex);
    mythread_cond(&count_threshold_cv);

    mythread_start(watch_count, NULL, 0);
    mythread_start(inc_count, "a", 10);
    mythread_start(inc_count, "b", 10);


    mythread_join();
    mythread_destroy(&count_mutex);
    mythread_destroy(&count_threshold_cv);

    printf("Threads exited\n");

    return 0;
}