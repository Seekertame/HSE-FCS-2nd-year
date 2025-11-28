cmake_minimum_required(VERSION 3.28)
project(AKOS_HW_8 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(server
        shared.h
        server.c
)
add_executable(client
        shared.h
        client.c
)

find_package(Threads REQUIRED)

target_link_libraries(server PRIVATE Threads::Threads rt)
target_link_libraries(client PRIVATE Threads::Threads rt)
