cmake_minimum_required(VERSION 3.28.3)

project(Threadpool
    LANGUAGES CXX
)

add_library(threadpool INTERFACE)
target_include_directories(threadpool INTERFACE .)

add_subdirectory(tests)
