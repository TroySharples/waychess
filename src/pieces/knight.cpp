#include "knight.hpp"

#include "details/ram.hpp"

namespace
{

const std::span<std::uint64_t> attack_table_knight = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_knight_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}

bitboard get_knight_attacked_squares_from_bitboard(bitboard b) noexcept
{
    bitboard ret {};

    // South-West-West.
    ret |= ((b >> 10) & ~(FILE_G | FILE_H));
    // North-West-West.
    ret |= ((b << 6) & ~(FILE_G | FILE_H));
    // North-North-West.
    ret |= ((b << 15) & ~FILE_H);
    // North-North-East.
    ret |= ((b << 17) & ~FILE_A);
    // North-East-East.
    ret |= ((b << 10) & ~(FILE_A | FILE_B));
    // South-East-East.
    ret |= ((b >> 6) & ~(FILE_A | FILE_B));
    // South-South-East.
    ret |= ((b >> 15) & ~FILE_A);
    // South-South-West.
    ret |= ((b >> 17) & ~FILE_H);

    return ret;
}

bitboard get_knight_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_knight[x];
}