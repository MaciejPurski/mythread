cmake_minimum_required(VERSION 3.15)
project(c_threads C)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)

add_executable(c_threads main.c mythread.h mythread.c thread.h mutex.c mutex.h cond.h cond.c)

add_executable(cond-test conditional-test.c mythread.h mythread.c thread.h mutex.c mutex.h cond.h cond.c)
