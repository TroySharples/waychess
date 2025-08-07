include(GoogleTest)

add_executable(test-perft ${CMAKE_CURRENT_SOURCE_DIR}/test_perft.cpp)
target_link_libraries(test-perft PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-perft)

add_executable(test-move-generation ${CMAKE_CURRENT_SOURCE_DIR}/test_move_generation.cpp)
target_link_libraries(test-move-generation PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-move-generation)

add_executable(test-puzzles ${CMAKE_CURRENT_SOURCE_DIR}/test_puzzles.cpp)
target_compile_definitions(test-puzzles PRIVATE PUZZLE_CSV_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../puzzles/mate5000.csv")
target_link_libraries(test-puzzles PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-puzzles)
