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

    // South-West.
    ret |= ((b >> 9) & ~FILE_H);
    // West.
    ret |= ((b >> 1) & ~FILE_H);
    // North-West.
    ret |= ((b << 7) & ~FILE_H);
    // North.
    ret |= (b << 8);
    // North-East.
    ret |= ((b << 9) & ~FILE_A);
    // East.
    ret |= ((b << 1) & ~FILE_A);
    // South-East.
    ret |= ((b >> 7) & ~FILE_A);
    // South.
    ret |= (b >> 8);

    return ret;
}

bitboard get_king_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_king[x];
}