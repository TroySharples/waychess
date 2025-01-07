#include "knight.hpp"

namespace
{

attack_table create_attack_table_knight()
{
    attack_table ret;

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_knight_attacked_squares(1ull << i);

    return ret;
}

}

bitboard get_knight_attacked_squares(bitboard b) noexcept
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

const attack_table attack_table_knight { create_attack_table_knight() };