configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

find_package(Threads REQUIRED)

add_library(lib-waychess STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/binary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/coordinates.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/perft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/puzzle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/uci.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/details/ram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/knight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/king.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/pawn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/rook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/bishop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/queen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/pieces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/bitboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/mailbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/move.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/game_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/search/statistics.cpp
)
target_include_directories(lib-waychess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(lib-waychess PUBLIC cxx_std_20)
target_link_libraries(lib-waychess PUBLIC Threads::Threads)
