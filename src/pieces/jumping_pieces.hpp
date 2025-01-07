#pragma once

#include "bitboard.hpp"

#include <array>

// Attack tables represent all pseudolegal attacking moves for a piece in a given position. Unlike for sliding pieces, there is no need to 
// consider the presence of other pieces on the board.

using attack_table = std::array<bitboard, 64>;

