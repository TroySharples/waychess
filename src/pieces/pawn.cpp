#include "pawn.hpp"

#include "details/ram.hpp"

namespace details
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