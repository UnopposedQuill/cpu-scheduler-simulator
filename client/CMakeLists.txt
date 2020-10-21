cmake_minimum_required(VERSION 3.17)
project(client C)

set(CMAKE_C_STANDARD 11)

add_executable(client main.c configuration.c configuration.h requester.c requester.h)

#I need this for pthread purposes
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)
