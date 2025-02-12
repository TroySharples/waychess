#pragma once

#include "piece.hpp"

// It can be convenient to consider white and black pawns as two different piece types as their directions of movment and attack are oposite.

bitboard get_white_pawn_attacked_squares(bitboard b) noexcept;
bitboard get_black_pawn_attacked_squares(bitboard b) noexcept;

extern const bitboard_table attack_table_white_pawn;
extern const bitboard_table attack_table_black_pawn;