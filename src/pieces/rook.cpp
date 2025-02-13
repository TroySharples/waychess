#include "rook.hpp"

#include "details/ram.hpp"
#include "details/pext_bitboard.hpp"

#include <array>

namespace
{

bitboard get_rook_xrayed_squares_from_mailbox_impl(mailbox x) noexcept
{
    return (get_bitboard_mailbox_rank(x) | get_bitboard_mailbox_file(x)) & ~get_bitboard_mailbox_piece(x);
}

const std::span<std::uint64_t> xray_table_rook = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_rook_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

bitboard get_rook_blocker_squares_from_mailbox_impl(mailbox x) noexcept
{
    return ((get_bitboard_mailbox_rank(x) & ~FILE_A & ~FILE_H) | (get_bitboard_mailbox_file(x) & ~RANK_1 & ~RANK_8)) & ~get_bitboard_mailbox_piece(x);
}

const std::span<std::uint64_t> blocker_table_rook = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_rook_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

bitboard get_rook_attacked_squares_from_mailbox_impl(mailbox x, bitboard pos)
{
    bitboard ret {};

    const bitboard rook { get_bitboard_mailbox_piece(x) };
    
    // Mask off the position of the rook itself in the bitboard - the presence of this screws with the blocking-piece calculation.
    pos &= ~rook;

    for (bitboard candidate = rook; (candidate & (FILE_A | pos)) == 0; candidate = shift_west(candidate) )
        ret |= shift_west(candidate);
    for (bitboard candidate = rook; (candidate & (RANK_8 | pos)) == 0; candidate = shift_north(candidate) )
        ret |= shift_north(candidate);
    for (bitboard candidate = rook; (candidate & (FILE_H | pos)) == 0; candidate = shift_east(candidate) )
        ret |= shift_east(candidate);
    for (bitboard candidate = rook; (candidate & (RANK_1 | pos)) == 0; candidate = shift_south(candidate) )
        ret |= shift_south(candidate);

    return ret;
}

const std::array<details::pext_bitboard, 64>  attack_table_rook = [] ()
{
    std::array<details::pext_bitboard, 64>  ret;

    for (mailbox i = 0; i < ret.size(); i++)
    {
        const bitboard blocker_squares { get_rook_blocker_squares_from_mailbox_impl(i) };
        const auto combinations = get_1s_combinations(blocker_squares);

        // Get a span from the shared RAM that is the correct size for this.
        ret[i].mask  = blocker_squares;
        ret[i].table = details::get_ram_slice(combinations.size());

        // Fill out the LUT properly.
        for (const auto comb : combinations)
            ret[i][comb] = get_rook_attacked_squares_from_mailbox_impl(i, comb);
    }

    return ret;
} ();

}

bitboard get_rook_xrayed_squares_from_mailbox(mailbox x) noexcept
{
    return xray_table_rook[x];
}

bitboard get_rook_blocker_squares_from_mailbox(mailbox x) noexcept
{
    return blocker_table_rook[x];
}

bitboard get_rook_attacked_squares_from_mailbox(mailbox x, bitboard pos) noexcept
{
    return attack_table_rook[x][pos];
}