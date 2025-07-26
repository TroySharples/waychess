#include "king.hpp"

#include "details/ram.hpp"

namespace details
{

const std::span<std::uint64_t> attack_table_king = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_king_attacked_squares_from_bitboard(1ULL << i);

    return ret;
} ();

}