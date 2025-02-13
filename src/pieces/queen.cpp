#include "queen.hpp"

#include "bishop.hpp"
#include "rook.hpp"

#include "details/ram.hpp"

#include <array>

namespace
{

bitboard get_queen_xrayed_squares_from_mailbox_impl(mailbox x) noexcept
{
    return get_rook_xrayed_squares_from_mailbox(x) | get_bishop_xrayed_squares_from_mailbox(x);
}

const std::span<std::uint64_t> xray_table_queen = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_queen_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

bitboard get_queen_blocker_squares_from_mailbox_impl(mailbox x) noexcept
{
    return get_rook_blocker_squares_from_mailbox(x) | get_bishop_blocker_squares_from_mailbox(x);
}

const std::span<std::uint64_t> blocker_table_queen = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (mailbox i = 0; i < ret.size(); i++)
        ret[i] = get_queen_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

}

bitboard get_queen_xrayed_squares_from_mailbox(mailbox x) noexcept
{
    return xray_table_queen[x];
}

bitboard get_queen_blocker_squares_from_mailbox(mailbox x) noexcept
{
    return blocker_table_queen[x];
}

bitboard get_queen_attacked_squares_from_mailbox(mailbox x, bitboard pos) noexcept
{
    return get_rook_attacked_squares_from_mailbox(x, pos) | get_bishop_attacked_squares_from_mailbox(x, pos);
}