#pragma once

#include "piece.hpp"

bitboard get_knight_attacked_squares(bitboard b) noexcept;

extern const bitboard_table attack_table_knight;