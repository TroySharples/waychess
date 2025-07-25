#pragma once

#include "bitboard.hpp"

struct make_move_args
{
    bool check_legality;
};

// We'll change this signature in the future when we come to implementing Zobrist hashing.
bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake) noexcept;
bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move) noexcept;

#include "details/make_move.hpp"
