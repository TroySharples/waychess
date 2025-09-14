#pragma once

// ####################################
// DECLARATION
// ####################################

#include "utility/binary.hpp"

// There are a few properties of pawns that make then a pain to code:
// * Their attack squares are not the same as their push squares.
// * They can push two squares on their first move, creating en-passent squares in the process.
// * The set of attacked squares also has to be intersected with the set of en-passent squares when calculating
//   attack moves.
// * They can only move in one direction, so it is convenient to consider white and black pawns as entirely
//   different piece types.
// * It is convenient to consider the two attacking directions separately so things like double-attacks and
//   single-attacks are calculated more easily.
// * Promotion.

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ATTACKS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

std::uint64_t get_white_pawn_west_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_white_pawn_east_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_white_pawn_all_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_white_pawn_single_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_white_pawn_double_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;

std::uint64_t get_white_pawn_all_attacked_squares_from_mailbox(std::size_t mb) noexcept;

inline std::uint64_t get_white_pawn_all_attacked_squares_from_mailboxes(auto... mbs) noexcept { return (get_white_pawn_all_attacked_squares_from_mailbox(mbs) | ...); }

std::uint64_t get_black_pawn_west_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_black_pawn_east_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_black_pawn_all_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_black_pawn_single_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_black_pawn_double_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;

std::uint64_t get_black_pawn_all_attacked_squares_from_mailbox(std::size_t mb) noexcept;

inline std::uint64_t get_black_pawn_all_attacked_squares_from_mailboxes(auto... mbs) noexcept { return (get_black_pawn_all_attacked_squares_from_mailbox(mbs) | ...); }

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// PUSHES
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

std::uint64_t get_white_pawn_single_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept;
std::uint64_t get_white_pawn_double_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept;
std::uint64_t get_white_pawn_all_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept;

std::uint64_t get_white_pawn_single_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept;
std::uint64_t get_white_pawn_double_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept;
std::uint64_t get_white_pawn_all_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept;

std::uint64_t get_black_pawn_single_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept;
std::uint64_t get_black_pawn_double_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept;
std::uint64_t get_black_pawn_all_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept;

std::uint64_t get_black_pawn_single_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept;
std::uint64_t get_black_pawn_double_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept;
std::uint64_t get_black_pawn_all_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept;

// ####################################
// IMPLEMENTATION
// ####################################

#include <span>

namespace details
{

extern const std::span<std::uint64_t> attack_table_white_pawn;
extern const std::span<std::uint64_t> attack_table_black_pawn;

}

inline std::uint64_t get_white_pawn_west_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_north_west(bb);
}

inline std::uint64_t get_white_pawn_east_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_north_east(bb);
}

inline std::uint64_t get_white_pawn_all_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(bb) | get_white_pawn_east_attacked_squares_from_bitboard(bb);
}

inline std::uint64_t get_white_pawn_single_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(bb) ^ get_white_pawn_east_attacked_squares_from_bitboard(bb);
}

inline std::uint64_t get_white_pawn_double_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(bb) & get_white_pawn_east_attacked_squares_from_bitboard(bb);
}

inline std::uint64_t get_white_pawn_all_attacked_squares_from_mailbox(std::size_t mb) noexcept
{
    return details::attack_table_white_pawn[mb];
}

inline std::uint64_t get_black_pawn_west_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_south_west(bb);
}

inline std::uint64_t get_black_pawn_east_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_south_east(bb);
}

inline std::uint64_t get_black_pawn_all_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(bb) | get_black_pawn_east_attacked_squares_from_bitboard(bb);
}

inline std::uint64_t get_black_pawn_single_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(bb) ^ get_black_pawn_east_attacked_squares_from_bitboard(bb);
}

inline std::uint64_t get_black_pawn_double_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(bb) & get_black_pawn_east_attacked_squares_from_bitboard(bb);
}

inline std::uint64_t get_black_pawn_all_attacked_squares_from_mailbox(std::size_t mb) noexcept
{
    return details::attack_table_black_pawn[mb];
}

inline std::uint64_t get_white_pawn_single_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (bb << 8) & npos;
}

inline std::uint64_t get_white_pawn_double_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (get_white_pawn_single_push_squares_from_bitboard(bb, npos) << 8) & npos & RANK_4;
}

inline std::uint64_t get_white_pawn_all_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(bb, npos) | get_white_pawn_double_push_squares_from_bitboard(bb, npos);
}

inline std::uint64_t get_white_pawn_single_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_white_pawn_double_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_white_pawn_all_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_black_pawn_single_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (bb >> 8) & npos;
}

inline std::uint64_t get_black_pawn_double_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (get_black_pawn_single_push_squares_from_bitboard(bb, npos) >> 8) & npos & RANK_5;
}

inline std::uint64_t get_black_pawn_all_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(bb, npos) | get_black_pawn_double_push_squares_from_bitboard(bb, npos);
}

inline std::uint64_t get_black_pawn_single_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_black_pawn_double_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_black_pawn_all_push_squares_from_mailbox(std::size_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}