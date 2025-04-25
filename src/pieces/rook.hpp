#pragma once

#include "utility/binary.hpp"

std::uint64_t get_rook_xrayed_squares_from_mailbox(std::size_t x) noexcept;
std::uint64_t get_rook_blocker_squares_from_mailbox(std::size_t x) noexcept;
std::uint64_t get_rook_attacked_squares_from_mailbox(std::size_t x, std::uint64_t pos) noexcept;