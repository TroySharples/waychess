add_library(lib-waychess STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bitboard.cpp
)
target_compile_features(lib-waychess PUBLIC cxx_std_23)
target_compile_options(lib-waychess PUBLIC -fconstexpr-depth=27 -fconstexpr-steps=127124200)

add_executable(waychess ${CMAKE_CURRENT_SOURCE_DIR}/waychess.cpp)
target_link_libraries(waychess PRIVATE lib-waychess)

