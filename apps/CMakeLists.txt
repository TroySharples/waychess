add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)

add_executable(perft-tree ${CMAKE_CURRENT_SOURCE_DIR}/perft_tree.cpp)
target_link_libraries(perft-tree PRIVATE lib-waychess)
