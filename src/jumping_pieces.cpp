#include "jumping_pieces.hpp"

#include "bitboard.hpp"

namespace
{

attack_table create_attack_table_king()
{
    attack_table ret {};

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const bitboard b { 1ull << i };

        // South-West.
        ret[i] |= ((b >> 9) & ~FILE_H);
        // West.
        ret[i] |= ((b >> 1) & ~FILE_H);
        // North-West.
        ret[i] |= ((b << 7) & ~FILE_H);
        // North.
        ret[i] |= (b << 8);
        // North-East.
        ret[i] |= ((b << 9) & ~FILE_A);
        // East.
        ret[i] |= ((b << 1) & ~FILE_A);
        // South-East.
        ret[i] |= ((b >> 7) & ~FILE_A);
        // South.
        ret[i] |= (b >> 8);
    }

    return ret;
}

attack_table create_attack_table_white_pawn()
{
    attack_table ret {};

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const bitboard b { 1ull << i };

        // North-West.
        ret[i] |= ((b << 7) & ~FILE_H);
        // North-East.
        ret[i] |= ((b << 9) & ~FILE_A);
    }

    return ret;
}

attack_table create_attack_table_black_pawn()
{
    attack_table ret {};

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const bitboard b { 1ull << i };

        // South-West.
        ret[i] |= ((b >> 9) & ~FILE_H);
        // South-East.
        ret[i] |= ((b >> 7) & ~FILE_A);
    }

    return ret;
}

}

const attack_table attack_table_king { create_attack_table_king() };
const attack_table attack_table_white_pawn { create_attack_table_white_pawn() };
const attack_table attack_table_black_pawn { create_attack_table_black_pawn() };