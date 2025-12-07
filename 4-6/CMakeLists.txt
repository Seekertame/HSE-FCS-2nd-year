add_executable(os_4_6
        src/main_4_6.c
        src/programmer_logic_4_6.c
        include/four_six/config_4_6.h
)

target_include_directories(os_4_6
        PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(os_4_6
        PRIVATE
        common
)
