#include "queen.hpp"

#include "bishop.hpp"
#include "rook.hpp"

#include "details/ram.hpp"

#include <array>

namespace
{

std::uint64_t get_queen_xrayed_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return get_rook_xrayed_squares_from_mailbox(mb) | get_bishop_xrayed_squares_from_mailbox(mb);
}

const std::span<std::uint64_t> xray_table_queen = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_queen_xrayed_squares_from_mailbox_impl(i);

    return ret;
} ();

std::uint64_t get_queen_blocker_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return get_rook_blocker_squares_from_mailbox(mb) | get_bishop_blocker_squares_from_mailbox(mb);
}

const std::span<std::uint64_t> blocker_table_queen = [] ()
{
    auto ret = details::get_ram_slice(64);

    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = get_queen_blocker_squares_from_mailbox_impl(i);

    return ret;
} ();

}

std::uint64_t get_queen_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return xray_table_queen[mb];
}

std::uint64_t get_queen_blocker_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return blocker_table_queen[mb];
}

std::uint64_t get_queen_attacked_squares_from_mailbox(std::uint8_t mb, std::uint64_t pos) noexcept
{
    return get_rook_attacked_squares_from_mailbox(mb, pos) | get_bishop_attacked_squares_from_mailbox(mb, pos);
}