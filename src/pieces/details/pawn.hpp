#pragma once

#include "pieces/pawn.hpp"

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

inline std::uint64_t get_white_pawn_all_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
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

inline std::uint64_t get_black_pawn_all_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
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

inline std::uint64_t get_white_pawn_single_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_white_pawn_double_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_white_pawn_all_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
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

inline std::uint64_t get_black_pawn_single_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_black_pawn_double_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

inline std::uint64_t get_black_pawn_all_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}