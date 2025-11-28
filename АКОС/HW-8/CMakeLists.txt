cmake_minimum_required(VERSION 3.10)
project(hw8_posix_shm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

add_executable(client client.c)
add_executable(server server.c)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# On some systems shm_open lives in librt, so link it when available.
if(UNIX AND NOT APPLE)
    find_library(RT_LIBRARY rt)
    if(RT_LIBRARY)
        target_link_libraries(client PRIVATE ${RT_LIBRARY})
        target_link_libraries(server PRIVATE ${RT_LIBRARY})
    endif()
endif()
