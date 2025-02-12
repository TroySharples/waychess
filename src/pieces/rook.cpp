#include "rook.hpp"
#include "board.hpp"
#include "details/ram.hpp"
#include "pieces/piece.hpp"

namespace
{

bitboard_table create_xray_table_rook()
{
    bitboard_table ret;

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_rook_xrayed_squares(1ull << i);

    return ret;
}

sliding_attack_table create_attack_table_rook()
{
    sliding_attack_table ret;

    for (mailbox i = 0; i < ret.size(); i++)
    {
        const bitboard blocker_squares { get_rook_blocker_squares(i) };
        const auto combinations = get_1s_combinations(blocker_squares);

        // Get a span from the shared RAM that is the correct size for this.
        ret[i].mask  = blocker_squares;
        ret[i].table = details::get_ram_slice(combinations.size());

        // Fill out the LUT properly.
        for (const auto comb : combinations)
            ret[i][comb] = get_rook_attacked_squares(i, comb);
    }

    return ret;
}

}

bitboard get_rook_xrayed_squares(mailbox x) noexcept
{
    return (get_bitboard_mailbox_rank(x) | get_bitboard_mailbox_file(x)) & ~get_bitboard_mailbox_piece(x);
}

bitboard get_rook_blocker_squares(mailbox x) noexcept
{
    return ((get_bitboard_mailbox_rank(x) & ~FILE_A & ~FILE_H) | (get_bitboard_mailbox_file(x) & ~RANK_1 & ~RANK_8)) & ~get_bitboard_mailbox_piece(x);
}

const bitboard_table xray_table_rook { create_xray_table_rook() };

bitboard get_rook_attacked_squares(mailbox x, bitboard pos)
{
    bitboard ret {};

    const bitboard rook { get_bitboard_mailbox_piece(x) };
    
    // Mask off the position of the rook itself in the bitboard - the presence of this screws with the blocking-piece calculation.
    pos &= ~rook;

    // West.
    for (bitboard candidate = rook; (candidate & (FILE_A | pos)) == 0; candidate = shift_west(candidate) )
        ret |= shift_west(candidate);
    // North.
    for (bitboard candidate = rook; (candidate & (RANK_8 | pos)) == 0; candidate = shift_north(candidate) )
        ret |= shift_north(candidate);
    // East.
    for (bitboard candidate = rook; (candidate & (FILE_H | pos)) == 0; candidate = shift_east(candidate) )
        ret |= shift_east(candidate);
    // South.
    for (bitboard candidate = rook; (candidate & (RANK_1 | pos)) == 0; candidate = shift_south(candidate) )
        ret |= shift_south(candidate);

    return ret;
}

const sliding_attack_table attack_table_rook { create_attack_table_rook() };