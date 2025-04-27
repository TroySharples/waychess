#pragma once

#include "bitboard.hpp"

// We'll change this signature in the future when we come to implementing Zobrist hashing.
void make_move(bitboard& bb, std::uint32_t move);
