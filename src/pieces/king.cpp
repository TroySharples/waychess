#include "king.hpp"

#include "details/ram.hpp"

namespace
{

const std::span<std::uint64_t> attack_table_king = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_king_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}

bitboard get_king_attacked_squares_from_bitboard(bitboard b) noexcept
{
    bitboard ret {};

    ret |= shift_south_west(b);
    ret |= shift_west(b);
    ret |= shift_north_west(b);
    ret |= shift_north(b);
    ret |= shift_north_east(b);
    ret |= shift_east(b);
    ret |= shift_south_east(b);
    ret |= shift_south(b);

    return ret;
}

bitboard get_king_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_king[x];
}