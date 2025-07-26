#pragma once

#include "utility/binary.hpp"

std::uint64_t get_king_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_king_attacked_squares_from_mailbox(std::uint8_t mb) noexcept;

inline std::uint64_t get_king_attacked_squares_from_mailboxes(auto... mbs) noexcept { return (get_king_attacked_squares_from_mailbox(mbs) | ...); }

#include "details/king.hpp"