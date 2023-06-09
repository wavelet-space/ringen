
cmake_minimum_required(VERSION 3.20)

project(
    Disruptor
    LANGUAGES CXX
)


add_executable(
    disruptor_example
    disruptor_example.cpp
)

target_include_directories(
    disruptor_example
    PRIVATE include
)