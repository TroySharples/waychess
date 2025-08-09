#pragma once

// ####################################
// DECLARATION
// ####################################

#include "utility/binary.hpp"

std::uint64_t get_bishop_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept;
std::uint64_t get_bishop_blocker_squares_from_mailbox(std::uint8_t mb) noexcept;
std::uint64_t get_bishop_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept;

inline std::uint64_t get_bishop_attacked_squares_from_mailboxes(std::uint64_t pos, auto... mbs) noexcept { return (get_bishop_attacked_squares_from_mailbox(pos, mbs) | ...); }

// ####################################
// IMPLEMENTATION
// ####################################

#include "details/pext_bitboard.hpp"

namespace details
{

inline std::uint64_t get_bishop_xrayed_squares_from_mailbox_impl(std::uint8_t mb) noexcept
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

extern const std::span<std::uint64_t> xray_table_bishop;

inline std::uint64_t get_bishop_blocker_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return get_bishop_xrayed_squares_from_mailbox_impl(mb) & ~(FILE_A | FILE_H | RANK_1 | RANK_8);
}

extern const std::span<std::uint64_t> blocker_table_bishop;

inline std::uint64_t get_bishop_attacked_squares_from_mailbox_impl(std::uint8_t mb, std::uint64_t pos)
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

extern const std::array<details::pext_bitboard, 64>  attack_table_bishop;

}

inline std::uint64_t get_bishop_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::xray_table_bishop[mb];
}

inline std::uint64_t get_bishop_blocker_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::blocker_table_bishop[mb];
}

inline std::uint64_t get_bishop_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept
{
    return details::attack_table_bishop[mb][pos];
}
