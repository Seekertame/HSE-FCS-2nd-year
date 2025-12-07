cmake_minimum_required(VERSION 3.28)
project(os_ihw C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(common)

add_subdirectory("4-6")
add_subdirectory("7-8")
add_subdirectory("9")
add_subdirectory("10")