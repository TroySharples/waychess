#pragma once

#include "pieces/queen.hpp"

#include "bishop.hpp"
#include "rook.hpp"

namespace details
{

inline std::uint64_t get_queen_xrayed_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return get_rook_xrayed_squares_from_mailbox(mb) | get_bishop_xrayed_squares_from_mailbox(mb);
}

extern const std::span<std::uint64_t> xray_table_queen;

inline std::uint64_t get_queen_blocker_squares_from_mailbox_impl(std::uint8_t mb) noexcept
{
    return get_rook_blocker_squares_from_mailbox(mb) | get_bishop_blocker_squares_from_mailbox(mb);
}

extern const std::span<std::uint64_t> blocker_table_queen;

}

inline std::uint64_t get_queen_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::xray_table_queen[mb];
}

inline std::uint64_t get_queen_blocker_squares_from_mailbox(std::uint8_t mb) noexcept
{
    return details::blocker_table_queen[mb];
}

inline std::uint64_t get_queen_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept
{
    return get_rook_attacked_squares_from_mailbox(pos, mb) | get_bishop_attacked_squares_from_mailbox(pos, mb);
}