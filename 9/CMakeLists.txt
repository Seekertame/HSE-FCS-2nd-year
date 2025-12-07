add_executable(os_9_programmer
        src/main_9_programmer.c
        src/programmer_proc_9.c
        ../7-8/src/programmer_proc_7_8.c
        ../7-8/src/ipc_7_8.c
)

add_executable(os_9_observer
        src/main_9_observer.c
        src/observer_9.c
)

target_include_directories(os_9_programmer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_SOURCE_DIR}/7-8/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(os_9_observer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(os_9_programmer
        PRIVATE
        common
)

target_link_libraries(os_9_observer
        PRIVATE
        common
)
