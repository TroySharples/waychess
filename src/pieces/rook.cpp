#include "rook.hpp"
#include "board.hpp"
#include "hashtable.hpp"
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
        const bitboard xrayed_squares { get_rook_xrayed_squares(i) };
        const auto combinations = get_1s_combinations(xrayed_squares);

        // Create our standard C++ map we can then use to create our magic bitboard table. We set all the bits in the key that we
        // don't care about to one (the black-magic bitboards trick). In simple fancy bitboards we would instead mask these off,
        // but doing it this way ends up reducing the size of the hash table we'll eventually have to calculate.
        ret[i].notmask = ~xrayed_squares;
        std::map<std::uint64_t, std::uint64_t> map;
        for (const auto comb : combinations)
            map[comb | ret[i].notmask] = get_rook_attacked_squares(i, comb);
        
        // Actually calculate our magic bitboard tables.
        // ret[i].table = hashtable(map, 18);
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