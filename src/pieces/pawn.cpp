#include "pawn.hpp"

#include "details/ram.hpp"

namespace
{

const std::span<std::uint64_t> attack_table_white_pawn = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_white_pawn_all_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

const std::span<std::uint64_t> attack_table_black_pawn = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_black_pawn_all_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}

std::uint64_t get_white_pawn_west_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_north_west(bb);
}

std::uint64_t get_white_pawn_east_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_north_east(bb);
}

std::uint64_t get_white_pawn_all_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(bb) | get_white_pawn_east_attacked_squares_from_bitboard(bb);
}

std::uint64_t get_white_pawn_single_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(bb) ^ get_white_pawn_east_attacked_squares_from_bitboard(bb);
}

std::uint64_t get_white_pawn_double_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(bb) & get_white_pawn_east_attacked_squares_from_bitboard(bb);
}

std::uint64_t get_white_pawn_all_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return attack_table_white_pawn[mb];
}

std::uint64_t get_black_pawn_west_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_north_west(bb);
}

std::uint64_t get_black_pawn_east_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return shift_north_east(bb);
}

std::uint64_t get_black_pawn_all_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(bb) | get_black_pawn_east_attacked_squares_from_bitboard(bb);
}

std::uint64_t get_black_pawn_single_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(bb) ^ get_black_pawn_east_attacked_squares_from_bitboard(bb);
}

std::uint64_t get_black_pawn_double_attacked_squares_from_bitboard(std::uint64_t bb) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(bb) & get_black_pawn_east_attacked_squares_from_bitboard(bb);
}

std::uint64_t get_black_pawn_all_attacked_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return attack_table_black_pawn[mb];
}

std::uint64_t get_white_pawn_single_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (bb << 8) & npos;
}

std::uint64_t get_white_pawn_double_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (get_white_pawn_single_push_squares_from_bitboard(bb, npos) << 8) & npos & RANK_4;
}

std::uint64_t get_white_pawn_all_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(bb, npos) | get_white_pawn_double_push_squares_from_bitboard(bb, npos);
}

std::uint64_t get_white_pawn_single_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

std::uint64_t get_white_pawn_double_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

std::uint64_t get_white_pawn_all_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_white_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

std::uint64_t get_black_pawn_single_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (bb << 8) & npos;
}

std::uint64_t get_black_pawn_double_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return (get_black_pawn_single_push_squares_from_bitboard(bb, npos) << 8) & npos & RANK_4;
}

std::uint64_t get_black_pawn_all_push_squares_from_bitboard(std::uint64_t bb, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(bb, npos) | get_black_pawn_double_push_squares_from_bitboard(bb, npos);
}

std::uint64_t get_black_pawn_single_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

std::uint64_t get_black_pawn_double_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}

std::uint64_t get_black_pawn_all_push_squares_from_mailbox(std::uint8_t mb, std::uint64_t npos) noexcept
{
    return get_black_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(mb), npos);
}