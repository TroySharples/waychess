add_library(lib-waychess STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bit_board.cpp
)

add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)

