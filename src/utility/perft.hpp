#pragma once

#include "position/bitboard.hpp"

// Note only allocates the log2_floor the entries.
void set_perft_hash_table_bytes(std::size_t bytes);
std::size_t get_perft_hash_table_bytes();

std::size_t perft(const bitboard& start, std::uint8_t depth);