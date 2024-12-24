#pragma once

#include "bitboard.hpp"

struct bitboard
{
    std::uint64_t white_pieces;
    std::uint64_t black_pieces;

    std::uint64_t kings;
    std::uint64_t queens;
    std::uint64_t rooks;
    std::uint64_t bishops;
    std::uint64_t knights;
    std::uint64_t pawns;
};
