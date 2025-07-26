#pragma once

#include "pieces/king.hpp"

#include <span>

namespace details
{

extern const std::span<std::uint64_t> attack_table_king;

}

inline std::uint64_t get_king_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    std::uint64_t ret {};

    ret |= shift_south_west(bb);
    ret |= shift_west(bb);
    ret |= shift_north_west(bb);
    ret |= shift_north(bb);
    ret |= shift_north_east(bb);
    ret |= shift_east(bb);
    ret |= shift_south_east(bb);
    ret |= shift_south(bb);

    return ret;
}

inline std::uint64_t get_king_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::attack_table_king[mb];
}