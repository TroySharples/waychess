include(GoogleTest)

add_executable(test-perft ${CMAKE_CURRENT_SOURCE_DIR}/test_perft.cpp)
target_link_libraries(test-perft PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-perft)

add_executable(test-unmake ${CMAKE_CURRENT_SOURCE_DIR}/test_unmake.cpp)
target_link_libraries(test-unmake PRIVATE lib-waychess gtest pthread)
gtest_discover_tests(test-unmake)
