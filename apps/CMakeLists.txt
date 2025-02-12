add_executable(table-generator ${CMAKE_CURRENT_SOURCE_DIR}/table_generator.cpp)
target_link_libraries(table-generator PRIVATE lib-waychess)

add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)
