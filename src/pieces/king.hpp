#pragma once

#include "piece.hpp"

bitboard get_king_attacked_squares(bitboard b) noexcept;

extern const bitboard_table attack_table_king;