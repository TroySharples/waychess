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

std::uint64_t get_white_pawn_west_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return shift_north_west(b);
}

std::uint64_t get_white_pawn_east_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return shift_north_east(b);
}

std::uint64_t get_white_pawn_all_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(b) | get_white_pawn_east_attacked_squares_from_bitboard(b);
}

std::uint64_t get_white_pawn_single_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(b) ^ get_white_pawn_east_attacked_squares_from_bitboard(b);
}

std::uint64_t get_white_pawn_double_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(b) & get_white_pawn_east_attacked_squares_from_bitboard(b);
}

std::uint64_t get_white_pawn_all_attacked_squares_from_mailbox(std::size_t x) noexcept
{
    return attack_table_white_pawn[x];
}

std::uint64_t get_black_pawn_west_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return shift_north_west(b);
}

std::uint64_t get_black_pawn_east_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return shift_north_east(b);
}

std::uint64_t get_black_pawn_all_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(b) | get_black_pawn_east_attacked_squares_from_bitboard(b);
}

std::uint64_t get_black_pawn_single_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(b) ^ get_black_pawn_east_attacked_squares_from_bitboard(b);
}

std::uint64_t get_black_pawn_double_attacked_squares_from_bitboard(std::uint64_t b) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(b) & get_black_pawn_east_attacked_squares_from_bitboard(b);
}

std::uint64_t get_black_pawn_all_attacked_squares_from_mailbox(std::size_t x) noexcept
{
    return attack_table_black_pawn[x];
}

std::uint64_t get_white_pawn_single_push_squares_from_bitboard(std::uint64_t b, std::uint64_t npos) noexcept
{
    return (b << 8) & npos;
}

std::uint64_t get_white_pawn_double_push_squares_from_bitboard(std::uint64_t b, std::uint64_t npos) noexcept
{
    return (get_white_pawn_single_push_squares_from_bitboard(b, npos) << 8) & npos & RANK_4;
}

std::uint64_t get_white_pawn_all_push_squares_from_bitboard(std::uint64_t b, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(b, npos) | get_white_pawn_double_push_squares_from_bitboard(b, npos);
}

std::uint64_t get_white_pawn_single_push_squares_from_mailbox(std::size_t x, std::uint64_t npos) noexcept
{
    return get_white_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(x), npos);
}

std::uint64_t get_white_pawn_double_push_squares_from_mailbox(std::size_t x, std::uint64_t npos) noexcept
{
    return get_white_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(x), npos);
}

std::uint64_t get_white_pawn_all_push_squares_from_mailbox(std::size_t x, std::uint64_t npos) noexcept
{
    return get_white_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(x), npos);
}

std::uint64_t get_black_pawn_single_push_squares_from_bitboard(std::uint64_t b, std::uint64_t npos) noexcept
{
    return (b << 8) & npos;
}

std::uint64_t get_black_pawn_double_push_squares_from_bitboard(std::uint64_t b, std::uint64_t npos) noexcept
{
    return (get_black_pawn_single_push_squares_from_bitboard(b, npos) << 8) & npos & RANK_4;
}

std::uint64_t get_black_pawn_all_push_squares_from_bitboard(std::uint64_t b, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(b, npos) | get_black_pawn_double_push_squares_from_bitboard(b, npos);
}

std::uint64_t get_black_pawn_single_push_squares_from_mailbox(std::size_t x, std::uint64_t npos) noexcept
{
    return get_black_pawn_single_push_squares_from_bitboard(get_bitboard_mailbox_piece(x), npos);
}

std::uint64_t get_black_pawn_double_push_squares_from_mailbox(std::size_t x, std::uint64_t npos) noexcept
{
    return get_black_pawn_double_push_squares_from_bitboard(get_bitboard_mailbox_piece(x), npos);
}

std::uint64_t get_black_pawn_all_push_squares_from_mailbox(std::size_t x, std::uint64_t npos) noexcept
{
    return get_black_pawn_all_push_squares_from_bitboard(get_bitboard_mailbox_piece(x), npos);
}