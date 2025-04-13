#include "pawn.hpp"

#include "board.hpp"
#include "details/ram.hpp"

namespace
{

const std::span<std::uint64_t> attack_table_white_pawn = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_white_pawn_all_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

const std::span<std::uint64_t> attack_table_black_pawn = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_black_pawn_all_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}

bitboard get_white_pawn_west_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return shift_north_west(b);
}

bitboard get_white_pawn_east_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return shift_north_east(b);
}

bitboard get_white_pawn_all_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(b) | get_white_pawn_east_attacked_squares_from_bitboard(b);
}

bitboard get_white_pawn_single_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(b) ^ get_white_pawn_east_attacked_squares_from_bitboard(b);
}

bitboard get_white_pawn_double_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return get_white_pawn_west_attacked_squares_from_bitboard(b) & get_white_pawn_east_attacked_squares_from_bitboard(b);
}

bitboard get_white_pawn_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_white_pawn[x];
}

bitboard get_black_pawn_west_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return shift_north_west(b);
}

bitboard get_black_pawn_east_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return shift_north_east(b);
}

bitboard get_black_pawn_all_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(b) | get_black_pawn_east_attacked_squares_from_bitboard(b);
}

bitboard get_black_pawn_single_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(b) ^ get_black_pawn_east_attacked_squares_from_bitboard(b);
}

bitboard get_black_pawn_double_attacked_squares_from_bitboard(bitboard b) noexcept
{
    return get_black_pawn_west_attacked_squares_from_bitboard(b) & get_black_pawn_east_attacked_squares_from_bitboard(b);
}

bitboard get_black_pawn_attacked_squares_from_mailbox(mailbox x) noexcept
{
    return attack_table_black_pawn[x];
}

bitboard get_white_pawn_single_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept
{
    return (b << 8) & npos;
}

bitboard get_white_pawn_double_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept
{
    return (get_white_pawn_single_push_squares_from_bitboard(b, npos) << 8) & npos & RANK_4;
}

bitboard get_black_pawn_single_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept
{
    return (b << 8) & npos;
}

bitboard get_black_pawn_double_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept
{
    return (get_black_pawn_single_push_squares_from_bitboard(b, npos) << 8) & npos & RANK_4;
}