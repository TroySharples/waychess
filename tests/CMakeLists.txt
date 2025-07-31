include(GoogleTest)

add_executable(test-perft ${CMAKE_CURRENT_SOURCE_DIR}/test_perft.cpp)
target_link_libraries(test-perft PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-perft)

add_executable(test-move-generation ${CMAKE_CURRENT_SOURCE_DIR}/test_move_generation.cpp)
target_link_libraries(test-move-generation PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-move-generation)
