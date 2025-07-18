#pragma once

#include "bitboard.hpp"

// Returns a bitboard of all pieces (of any colour) attacking a particular square.
std::uint64_t get_attackers(const bitboard& bb, std::uint8_t mb);

// Returns a bitboard of all white pieces attacking a square.
std::uint64_t get_attackers_white(const bitboard& bb, std::uint8_t mb);

// Returns a bitboard of all black pieces attacking a square.
std::uint64_t get_attackers_black(const bitboard& bb, std::uint8_t mb);


