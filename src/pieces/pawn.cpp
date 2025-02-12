#include "pawn.hpp"

namespace
{

bitboard_table create_attack_table_white_pawn()
{
    bitboard_table ret;

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_white_pawn_attacked_squares(1ull << i);

    return ret;
}

bitboard_table create_attack_table_black_pawn()
{
    bitboard_table ret;

    for (mailbox i = 0; i < ret.size(); i++)
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

const bitboard_table attack_table_white_pawn { create_attack_table_white_pawn() };
const bitboard_table attack_table_black_pawn { create_attack_table_black_pawn() };