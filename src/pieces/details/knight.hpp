#pragma once

#include "pieces/knight.hpp"

#include <span>

namespace details
{

extern const std::span<std::uint64_t> attack_table_knight;

}

inline std::uint64_t get_knight_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    std::uint64_t ret {};

    // South-West-West.
    ret |= ((bb >> 10) & ~(FILE_G | FILE_H));
    // North-West-West.
    ret |= ((bb << 6) & ~(FILE_G | FILE_H));
    // North-North-West.
    ret |= ((bb << 15) & ~FILE_H);
    // North-North-East.
    ret |= ((bb << 17) & ~FILE_A);
    // North-East-East.
    ret |= ((bb << 10) & ~(FILE_A | FILE_B));
    // South-East-East.
    ret |= ((bb >> 6) & ~(FILE_A | FILE_B));
    // South-South-East.
    ret |= ((bb >> 15) & ~FILE_A);
    // South-South-West.
    ret |= ((bb >> 17) & ~FILE_H);

    return ret;
}

inline std::uint64_t get_knight_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::attack_table_knight[mb];
}