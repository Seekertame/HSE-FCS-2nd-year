add_executable(os_7_8_init
        src/main_7_8_init.c
        src/ipc_7_8.c
)

add_executable(os_7_8_programmer
        src/main_7_8_programmer.c
        src/programmer_proc_7_8.c
        src/ipc_7_8.c
)

target_include_directories(os_7_8_init
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(os_7_8_programmer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(os_7_8_init
        PRIVATE
        common
)

target_link_libraries(os_7_8_programmer
        PRIVATE
        common
)
