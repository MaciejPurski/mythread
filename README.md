# mythread
The aim of this project was to demonstrate one of the ways of implementing user-space threads in C. Context switching has been implemented using SYS-V setcontext(), getcontext() etc. A scheduler is implemented as a seperate thread, which is activated periodically, when SIGALRM signal arrives. Each thread has its priority, the smaller number, the greater priority. Scheduling works using a priority queue, preempting a thread on each call, only if there is a thread waiting with the same or higher priority. Basic synchronization mechanisms have also been implemented, e.t. mutex and conditional variables.

## Project structure
Threads, mutexes, and conditional variables have been implemented as a C dynamic library. There are also 3 exemplary programms provided. The structure is as follows:
* `src/` library source code and its private headers.
* `include/mythread/` public headers.
* `examples/` example programms.

## Build and run
This project uses CMake. The commands for building the project and running examples are:

```cmake .
make
./threads-test```

## Examples
There are three example programms included in this project:
* `threads-test` creates 7 threads, which periodically print messages. This example demonstrates working of a scheduler based on priority queue. Highest priority threads are executed first, however if there are more threads with the same priority, they get preempted.
* `mutex-test` works in a similar way to the previous one, but in this case, each thread holds a mutex when printing its messages.
* `cond-test` shows a sample usage of conditional variables. Two threads update one integer variable `count`, which is guarded by a mutex. A third thread watches value of the variable and waits on the conditional variable until it gets a desired value.

## Limitations
Because it is just a demo implementation, there are several limitations which should be noted.

According to [libc manual](https://www.gnu.org/software/libc/manual/html_node/System-V-contexts.html):
>It is not recommended to do the context switching from the signal handler directly since leaving the signal handler via setcontext if the signal was delivered during code that was not asynchronous signal safe could lead to problems.

However for simplicity, in order to emulate `pthread` API and to minimize user-side code I still do the context switching in a signal handler. The safest way would be to change some variable in a signal handler and call the scheduler in a user's thread, if the variable has been change. This would require additional code on the user side. A different workaround would be to block signals in a thread function when permorming asynchronous unsafe operations.


To keep things simple, I only implemented a function `mythread_join`, which causes a main thread to wait for all other threads and starts their execution. There are is no way to specify for which thread to wait.
