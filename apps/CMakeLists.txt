add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)

add_executable(waychess-perft ${CMAKE_CURRENT_SOURCE_DIR}/perft.cpp)
target_link_libraries(waychess-perft PRIVATE lib-waychess)
