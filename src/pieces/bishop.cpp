#include "bishop.hpp"

#include "details/ram.hpp"
#include "details/pext_bitboard.hpp"

#include <array>

namespace 
{

std::uint64_t get_bishop_xrayed_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    const std::uint64_t b { get_bitboard_mailbox_piece(mb) };

    std::uint64_t sw_ray {b}, nw_ray {b}, ne_ray {b}, se_ray {b};
    for (std::uint8_t i = 0; i < 7; i++)
    {
        sw_ray = get_bitboard_mailbox_piece(mb) | shift_south_west(sw_ray);
        nw_ray = get_bitboard_mailbox_piece(mb) | shift_north_west(nw_ray);
        ne_ray = get_bitboard_mailbox_piece(mb) | shift_north_east(ne_ray);
        se_ray = get_bitboard_mailbox_piece(mb) | shift_south_east(se_ray);
    }
    
    return (sw_ray | nw_ray | ne_ray | se_ray) & ~b;
}

const std::span<std::uint64_t> xray_table_bishop = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_bishop_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

std::uint64_t get_bishop_blocker_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return get_bishop_xrayed_squares_from_mailbox_impl(mb) & ~(FILE_A | FILE_H | RANK_1 | RANK_8);
}

const std::span<std::uint64_t> blocker_table_bishop = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_bishop_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

std::uint64_t get_bishop_attacked_squares_from_mailbox_impl(std::uint8_t mb, std::uint64_t pos)
{
    std::uint64_t ret {};

    const std::uint64_t bishop { get_bitboard_mailbox_piece(mb) };
    
    // Mask off the position of the bishop itself in the bitboard - the presence of this screws with the blocking-piece calculation.
    pos &= ~bishop;

    for (std::uint64_t candidate = bishop; (candidate & (FILE_A | RANK_1 | pos)) == 0; candidate = shift_south_west(candidate) )
        ret |= shift_south_west(candidate);
    for (std::uint64_t candidate = bishop; (candidate & (FILE_A | RANK_8 | pos)) == 0; candidate = shift_north_west(candidate) )
        ret |= shift_north_west(candidate);
    for (std::uint64_t candidate = bishop; (candidate & (FILE_H | RANK_8 | pos)) == 0; candidate = shift_north_east(candidate) )
        ret |= shift_north_east(candidate);
    for (std::uint64_t candidate = bishop; (candidate & (FILE_H | RANK_1 | pos)) == 0; candidate = shift_south_east(candidate) )
        ret |= shift_south_east(candidate);

    return ret;
}

const std::array<details::pext_bitboard, 64>  attack_table_bishop = [] ()
{
    std::array<details::pext_bitboard, 64>  ret;

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const std::uint64_t blocker_squares { get_bishop_blocker_squares_from_mailbox_impl(i) };
        const auto combinations = get_1s_combinations(blocker_squares);

        // Get a span from the shared RAM that is the correct size for this.
        ret[i].mask  = blocker_squares;
        ret[i].table = details::get_ram_slice(combinations.size());

        // Fill out the LUT properly.
        for (const auto comb : combinations)
            ret[i][comb] = get_bishop_attacked_squares_from_mailbox_impl(i, comb);
    }

    return ret;
} ();

}

std::uint64_t get_bishop_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return xray_table_bishop[mb];
}

std::uint64_t get_bishop_blocker_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return blocker_table_bishop[mb];
}

std::uint64_t get_bishop_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept
{
    return attack_table_bishop[mb][pos];
}