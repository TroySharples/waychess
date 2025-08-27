include(GoogleTest)

add_executable(test-perft ${CMAKE_CURRENT_SOURCE_DIR}/test_perft.cpp)
target_link_libraries(test-perft PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-perft)

add_executable(test-puzzles ${CMAKE_CURRENT_SOURCE_DIR}/test_puzzles.cpp)
target_compile_definitions(test-puzzles PRIVATE PUZZLE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../puzzles/")
target_link_libraries(test-puzzles PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-puzzles)
