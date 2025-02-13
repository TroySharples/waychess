#pragma once

#include "board.hpp"

// It can be convenient to consider white and black pawns as two different piece types as their directions of movment and attack are oposite.

bitboard get_white_pawn_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_white_pawn_attacked_squares_from_mailbox(mailbox b) noexcept;

bitboard get_black_pawn_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_black_pawn_attacked_squares_from_mailbox(mailbox b) noexcept;