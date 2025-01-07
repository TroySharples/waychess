#pragma once

#include "jumping_pieces.hpp"

bitboard get_knight_attacked_squares(bitboard b) noexcept;

extern const attack_table attack_table_knight;