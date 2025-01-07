#include "king.hpp"

namespace
{

attack_table create_attack_table_king()
{
    attack_table ret;

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_king_attacked_squares(1ull << i);

    return ret;
}

}

bitboard get_king_attacked_squares(bitboard b) noexcept
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

const attack_table attack_table_king { create_attack_table_king() };