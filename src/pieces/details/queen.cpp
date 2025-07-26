#include "queen.hpp"

#include "details/ram.hpp"

namespace details
{

const std::span<std::uint64_t> xray_table_queen = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_queen_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

const std::span<std::uint64_t> blocker_table_queen = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_queen_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

}