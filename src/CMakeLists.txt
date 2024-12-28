add_library(lib-waychess STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bitboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hashmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jumping_pieces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/knight.cpp
)
target_compile_features(lib-waychess PUBLIC cxx_std_23)

add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)

