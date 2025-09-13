#include "bishop.hpp"

#include "details/ram.hpp"

namespace details
{

const std::span<std::uint64_t> xray_table_bishop = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_bishop_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

const std::span<std::uint64_t> blocker_table_bishop = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_bishop_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

const std::array<details::pext_bitboard, 64>  attack_table_bishop = [] ()
{
    std::array<details::pext_bitboard, 64>  ret;

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const std::uint64_t blocker_squares { get_bishop_blocker_squares_from_mailbox_impl(i) };

        // Get a span from the shared RAM that is the correct size for this.
        ret[i].mask  = blocker_squares;
        ret[i].table = details::get_ram_slice(1ULL << std::popcount(blocker_squares));

        // Fill out the LUT by iterating over all subsets of the blocker squares
        std::uint64_t subset = 0;
        do
        {
            ret[i][subset] = get_bishop_attacked_squares_from_mailbox_impl(i, subset);
            subset = (subset - blocker_squares) & blocker_squares;
        }
        while (subset);
    }

    return ret;
} ();

}