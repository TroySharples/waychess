#pragma once

#include "bitboard.hpp"

struct make_move_args
{
    bool check_legality;
};

bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move) noexcept;
bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake) noexcept;
bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake, std::uint64_t& hash) noexcept;

void unmake_move(bitboard& bb, std::uint32_t& unmake) noexcept;
void unmake_move(bitboard& bb, std::uint32_t& unmake, std::uint64_t& hash) noexcept;

#include "details/make_move.hpp"
