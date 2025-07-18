#pragma once

#include "utility/binary.hpp"

std::uint64_t get_knight_attacked_squares_from_bitboard(std::uint64_t bb) noexcept;
std::uint64_t get_knight_attacked_squares_from_mailbox(std::uint8_t mb) noexcept;