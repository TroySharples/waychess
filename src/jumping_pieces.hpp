#pragma once

#include "bitboard.hpp"

#include <cstdint>
#include <array>

using attack_table = std::array<bitboard, 64>;

extern const attack_table attack_table_king;
extern const attack_table attack_table_white_pawn;
extern const attack_table attack_table_black_pawn;

