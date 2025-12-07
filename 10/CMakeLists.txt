add_executable(os_10_programmer
        src/main_10_programmer.c
        src/programmer_proc_10.c
        ../7-8/src/programmer_proc_7_8.c
        ../7-8/src/ipc_7_8.c
)

add_executable(os_10_observer
        src/main_10_observer.c
        src/observer_10.c
)

target_include_directories(os_10_programmer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_SOURCE_DIR}/7-8/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(os_10_observer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(os_10_programmer
        PRIVATE
        common
)

target_link_libraries(os_10_observer
        PRIVATE
        common
)
