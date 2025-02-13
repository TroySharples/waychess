#include "pawn.hpp"

#include "details/ram.hpp"

namespace
{

const std::span<std::uint64_t> attack_table_white_pawn = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_white_pawn_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

const std::span<std::uint64_t> attack_table_black_pawn = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_black_pawn_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}

bitboard get_white_pawn_attacked_squares_from_bitboard(bitboard b) noexcept
{
    bitboard ret {};

    // North-West.
    ret |= ((b << 7) & ~FILE_H);
    // North-East.
    ret |= ((b << 9) & ~FILE_A);

    return ret;
}

bitboard get_white_pawn_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_white_pawn[x];
}

bitboard get_black_pawn_attacked_squares_from_bitboard(bitboard b) noexcept
{
    bitboard ret {};

    // South-West.
    ret |= ((b >> 9) & ~FILE_H);
    // South-East.
    ret |= ((b >> 7) & ~FILE_A);

    return ret;
}

bitboard get_black_pawn_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_black_pawn[x];
}
