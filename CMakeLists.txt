cmake_minimum_required(VERSION 3.9)
project(ecoroutine)

set(CMAKE_CXX_STANDARD 17)

link_libraries(pthread)
add_executable(ecoroutine example.cc coroutine.cc coroutine.h benchmark.cc)
add_executable(benchmark benchmark.cc coroutine.cc coroutine.h)