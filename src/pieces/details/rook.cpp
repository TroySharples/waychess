#include "rook.hpp"

#include "details/ram.hpp"

namespace details
{

const std::span<std::uint64_t> xray_table_rook = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_rook_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

const std::span<std::uint64_t> blocker_table_rook = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_rook_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

const std::array<details::pext_bitboard, 64>  attack_table_rook = [] ()
{
    std::array<details::pext_bitboard, 64>  ret;

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const std::uint64_t blocker_squares { get_rook_blocker_squares_from_mailbox_impl(i) };
        const auto combinations = get_1s_combinations(blocker_squares);

        // Get a span from the shared RAM that is the correct size for this.
        ret[i].mask  = blocker_squares;
        ret[i].table = details::get_ram_slice(combinations.size());

        // Fill out the LUT properly.
        for (const auto comb : combinations)
            ret[i][comb] = get_rook_attacked_squares_from_mailbox_impl(i, comb);
    }

    return ret;
} ();

}