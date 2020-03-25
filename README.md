# mythread
The aim of this project was to demonstrate one of the ways of implementing user-space threads in C. Context switching has been implemented using SYS-V setcontext(), getcontext() etc. A scheduler is implemented as a seperate thread, which is activated periodically, when SIGALRM signal arrives. Each thread has its priority, the smaller number, the greater priority. Scheduling works using a priority queue, preempting a thread on each call, only if there is a thread waiting with the same or higher priority. Basic synchronization mechanisms have also been implemented, e.t. mutex and conditional variables.

## Project structure
Threads, mutexes, and conditional variables have been implemented as a C dynamic library. There are also 3 exemplary programms provided. The structure is as follows:
* `src/` library source code and its private headers.
* `include/mythread/` public headers.
* `examples/` example programms.

