#include "king.hpp"

#include "details/ram.hpp"

namespace
{

const std::span<std::uint64_t> attack_table_king = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_king_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}

std::uint64_t get_king_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
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

std::uint64_t get_king_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return attack_table_king[mb];
}