#pragma once

// ####################################
// DECLARATION
// ####################################

#include "utility/binary.hpp"

std::uint64_t get_queen_xrayed_squares_from_mailbox(std::size_t mb) noexcept;
std::uint64_t get_queen_blocker_squares_from_mailbox(std::size_t mb) noexcept;
std::uint64_t get_queen_attacked_squares_from_mailbox(std::uint64_t pos, std::size_t mb) noexcept;

inline std::uint64_t get_queen_attacked_squares_from_mailboxes(std::uint64_t pos, auto... mbs) noexcept { return (get_queen_attacked_squares_from_mailbox(pos, mbs) | ...); }

// ####################################
// IMPLEMENTATION
// ####################################

#include "bishop.hpp"
#include "rook.hpp"

namespace details
{

inline std::uint64_t get_queen_xrayed_squares_from_mailbox_impl(std::size_t mb) noexcept
{
    return get_rook_xrayed_squares_from_mailbox(mb) | get_bishop_xrayed_squares_from_mailbox(mb);
}

extern const std::span<std::uint64_t> xray_table_queen;

inline std::uint64_t get_queen_blocker_squares_from_mailbox_impl(std::size_t mb) noexcept
{
    return get_rook_blocker_squares_from_mailbox(mb) | get_bishop_blocker_squares_from_mailbox(mb);
}

extern const std::span<std::uint64_t> blocker_table_queen;

}

inline std::uint64_t get_queen_xrayed_squares_from_mailbox(std::size_t mb) noexcept
{
    return details::xray_table_queen[mb];
}

inline std::uint64_t get_queen_blocker_squares_from_mailbox(std::size_t mb) noexcept
{
    return details::blocker_table_queen[mb];
}

inline std::uint64_t get_queen_attacked_squares_from_mailbox(std::uint64_t pos, std::size_t mb) noexcept
{
    return get_rook_attacked_squares_from_mailbox(pos, mb) | get_bishop_attacked_squares_from_mailbox(pos, mb);
}