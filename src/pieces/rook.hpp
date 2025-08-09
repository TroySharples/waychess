#pragma once

// ####################################
// DECLARATION
// ####################################

#include "utility/binary.hpp"

std::uint64_t get_rook_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept;
std::uint64_t get_rook_blocker_squares_from_mailbox(std::uint8_t mb) noexcept;
std::uint64_t get_rook_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept;

inline std::uint64_t get_rook_attacked_squares_from_mailboxes(std::uint64_t pos, auto... mbs) noexcept { return (get_rook_attacked_squares_from_mailbox(pos, mbs) | ...); }

// ####################################
// IMPLEMENTATION
// ####################################

#include "pieces/rook.hpp"

#include "details/pext_bitboard.hpp"

namespace details
{

inline std::uint64_t get_rook_xrayed_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return (get_bitboard_mailbox_rank(mb) | get_bitboard_mailbox_file(mb)) & ~get_bitboard_mailbox_piece(mb);
}

extern const std::span<std::uint64_t> xray_table_rook;

inline std::uint64_t get_rook_blocker_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return ((get_bitboard_mailbox_rank(mb) & ~FILE_A & ~FILE_H) | (get_bitboard_mailbox_file(mb) & ~RANK_1 & ~RANK_8)) & ~get_bitboard_mailbox_piece(mb);
}

extern const std::span<std::uint64_t> blocker_table_rook;

inline std::uint64_t get_rook_attacked_squares_from_mailbox_impl(std::uint8_t mb, std::uint64_t pos)
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

extern const std::array<details::pext_bitboard, 64>  attack_table_rook;

}

inline std::uint64_t get_rook_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::xray_table_rook[mb];
}

inline std::uint64_t get_rook_blocker_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::blocker_table_rook[mb];
}

inline std::uint64_t get_rook_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept
{
    return details::attack_table_rook[mb][pos];
}