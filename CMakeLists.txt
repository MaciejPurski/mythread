cmake_minimum_required(VERSION 3.15)
project(c_threads C)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic ")


# Build a library
FILE(GLOB mythreadsrc ./src/*)
ADD_LIBRARY(mythreadlib STATIC ${mythreadsrc})
TARGET_INCLUDE_DIRECTORIES(mythreadlib PRIVATE ./src/ ./include/)


INCLUDE_DIRECTORIES(./include/)

ADD_EXECUTABLE(threads-test main.c)
TARGET_LINK_LIBRARIES(threads-test mythreadlib)

ADD_EXECUTABLE(cond-test conditional-test.c src/utils.c)
TARGET_LINK_LIBRARIES(cond-test mythreadlib)
