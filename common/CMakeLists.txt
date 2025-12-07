add_library(common STATIC
        src/model.c
        src/shared_state.c
        src/random_utils.c
        src/log_utils.c
        include/common/config.h
)

target_include_directories(common
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(common
        PUBLIC
        pthread
)
