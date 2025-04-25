#pragma once

#include "utility/binary.hpp"

std::uint64_t get_knight_attacked_squares_from_bitboard(std::uint64_t b) noexcept;
std::uint64_t get_knight_attacked_squares_from_mailbox(std::size_t b) noexcept;