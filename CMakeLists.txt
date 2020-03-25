CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(c_threads C)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")


# Build a library
FILE(GLOB mythreadsrc ./src/*)
ADD_LIBRARY(mythreadlib STATIC ${mythreadsrc})
TARGET_INCLUDE_DIRECTORIES(mythreadlib PRIVATE ./src/ ./include/)


INCLUDE_DIRECTORIES(./include/)

ADD_EXECUTABLE(mutex-test mutex-test.c)
TARGET_LINK_LIBRARIES(mutex-test mythreadlib)

ADD_EXECUTABLE(cond-test conditional-test.c)
TARGET_LINK_LIBRARIES(cond-test mythreadlib)

ADD_EXECUTABLE(threads-test threads-test.c)
TARGET_LINK_LIBRARIES(threads-test mythreadlib)
