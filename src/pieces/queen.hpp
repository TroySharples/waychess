#pragma once

#include "utility/binary.hpp"

std::uint64_t get_queen_xrayed_squares_from_mailbox(std::uint8_t mb) noexcept;
std::uint64_t get_queen_blocker_squares_from_mailbox(std::uint8_t mb) noexcept;
std::uint64_t get_queen_attacked_squares_from_mailbox(std::uint64_t pos, std::uint8_t mb) noexcept;

inline std::uint64_t get_queen_attacked_squares_from_mailboxes(std::uint64_t pos, auto... mbs) noexcept { return (get_queen_attacked_squares_from_mailbox(pos, mbs) | ...); }