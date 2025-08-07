add_library(lib-waychess STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/binary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/coordinates.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/perft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility/puzzle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/details/ram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/details/knight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/details/king.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/details/pawn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/details/rook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/details/bishop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/details/queen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pieces/pieces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/bitboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/mailbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/position/move.cpp
)
target_include_directories(lib-waychess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(lib-waychess PUBLIC cxx_std_20)
target_compile_options(lib-waychess PUBLIC -march=native)
