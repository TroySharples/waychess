#include "pawn.hpp"

namespace
{

attack_table create_attack_table_white_pawn()
{
    attack_table ret;

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_white_pawn_attacked_squares(1ull << i);

    return ret;
}

attack_table create_attack_table_black_pawn()
{
    attack_table ret;

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_black_pawn_attacked_squares(1ull << i);

    return ret;
}

}

bitboard get_white_pawn_attacked_squares(bitboard b) noexcept
{
    bitboard ret {};

    // North-West.
    ret |= ((b << 7) & ~FILE_H);
    // North-East.
    ret |= ((b << 9) & ~FILE_A);

    return ret;
}

bitboard get_black_pawn_attacked_squares(bitboard b) noexcept
{
    bitboard ret {};

    // South-West.
    ret |= ((b >> 9) & ~FILE_H);
    // South-East.
    ret |= ((b >> 7) & ~FILE_A);

    return ret;
}

const attack_table attack_table_white_pawn { create_attack_table_white_pawn() };
const attack_table attack_table_black_pawn { create_attack_table_black_pawn() };