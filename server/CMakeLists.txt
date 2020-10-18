cmake_minimum_required(VERSION 3.17)
project(server C)

set(CMAKE_C_STANDARD 11)

add_executable(server main.c configuration.c configuration.h pcb.c pcb.h)

#I need this for pthread purposes
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)