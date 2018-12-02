cmake_minimum_required(VERSION 3.9)
project(ecoroutine)

set(CMAKE_CXX_STANDARD 17)

add_executable(ecoroutine main.cpp coroutine.cc coroutine.h)