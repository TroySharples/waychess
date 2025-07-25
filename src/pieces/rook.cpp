#include "rook.hpp"

#include "details/ram.hpp"
#include "details/pext_bitboard.hpp"

#include <array>

namespace
{

std::uint64_t get_rook_xrayed_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return (get_bitboard_mailbox_rank(mb) | get_bitboard_mailbox_file(mb)) & ~get_bitboard_mailbox_piece(mb);
}

const std::span<std::uint64_t> xray_table_rook = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_rook_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

std::uint64_t get_rook_blocker_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return ((get_bitboard_mailbox_rank(mb) & ~FILE_A & ~FILE_H) | (get_bitboard_mailbox_file(mb) & ~RANK_1 & ~RANK_8)) & ~get_bitboard_mailbox_piece(mb);
}

const std::span<std::uint64_t> blocker_table_rook = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_rook_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

std::uint64_t get_rook_attacked_squares_from_mailbox_impl(std::uint8_t mb, std::uint64_t pos)
{
    std::uint64_t ret {};

    const std::uint64_t rook { get_bitboard_mailbox_piece(mb) };
    
    // Mask off the position of the rook itself in the bitboard - the presence of this screws with the blocking-piece calculation.
    pos &= ~rook;

    for (std::uint64_t candidate = rook; (candidate & (FILE_A | pos)) == 0; candidate = shift_west(candidate) )
        ret |= shift_west(candidate);
    for (std::uint64_t candidate = rook; (candidate & (RANK_8 | pos)) == 0; candidate = shift_north(candidate) )
        ret |= shift_north(candidate);
    for (std::uint64_t candidate = rook; (candidate & (FILE_H | pos)) == 0; candidate = shift_east(candidate) )
        ret |= shift_east(candidate);
    for (std::uint64_t candidate = rook; (candidate & (RANK_1 | pos)) == 0; candidate = shift_south(candidate) )
        ret |= shift_south(candidate);

    return ret;
}

const std::array<details::pext_bitboard, 64>  attack_table_rook = [] ()
{
    std::array<details::pext_bitboard, 64>  ret;

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const std::uint64_t blocker_squares { get_rook_blocker_squares_from_mailbox_impl(i) };
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

std::uint64_t get_rook_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return xray_table_rook[mb];
}

std::uint64_t get_rook_blocker_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return blocker_table_rook[mb];
}

std::uint64_t get_rook_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept
{
    return attack_table_rook[mb][pos];
}