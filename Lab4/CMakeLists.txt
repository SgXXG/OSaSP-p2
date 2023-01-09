cmake_minimum_required(VERSION 3.23)
project(Laba4)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Laba4
        TaskQueue.cpp
        TaskQueue.h
        TestManager.cpp
        TextFileSort.cpp
        ThreadManager.cpp
        ThreadManager.h)
