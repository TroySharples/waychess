#pragma once

#include "board.hpp"

// There are a few properties of pawns that make then a pain to code:
// * Their attack squares are not the same as their push squares.
// * They can push two squares on their first move, creating en-passent squares in the process.
// * The set of attacked squares also has to be intersected with the set of en-passent squares when calculating
//   attack moves.
// * They can only move in one direction, so it is convenient to consider white and black pawns as entirely
//   different piece types.
// * It is convenient to consider the two attacking directions separately so things like double-attacks and
//   single-attacks are calculated more easily.
// * Promotion.

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ATTACKS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

bitboard get_white_pawn_west_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_white_pawn_east_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_white_pawn_all_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_white_pawn_single_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_white_pawn_double_attacked_squares_from_bitboard(bitboard b) noexcept;

bitboard get_white_pawn_attacked_squares_from_mailbox(mailbox b) noexcept;

bitboard get_black_pawn_west_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_black_pawn_east_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_black_pawn_all_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_black_pawn_single_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_black_pawn_double_attacked_squares_from_bitboard(bitboard b) noexcept;

bitboard get_black_pawn_attacked_squares_from_mailbox(mailbox b) noexcept;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// PUSHES
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

bitboard get_white_pawn_single_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept;
bitboard get_white_pawn_double_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept;

bitboard get_black_pawn_single_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept;
bitboard get_black_pawn_double_push_squares_from_bitboard(bitboard b, bitboard npos) noexcept;