#pragma once

#include "king.hpp"
#include "pawn.hpp"
#include "knight.hpp"
#include "rook.hpp"
#include "bishop.hpp"
#include "queen.hpp"

// This types is also used to index our array of bitboards. We don't actually maintain a
// bitboard for empty squares at the moment - this is only included for easier serialisation
// and deserialisation.
enum piece_idx : std::uint8_t
{
    w_pawn    = 0x00,
    w_knight  = 0x01,
    w_rook    = 0x02,
    w_bishop  = 0x03,
    w_king    = 0x04,
    w_queen   = 0x05,
    b_pawn    = 0x06,
    b_knight  = 0x07,
    b_rook    = 0x08,
    b_bishop  = 0x09,
    b_king    = 0x0a,
    b_queen   = 0x0b,
    empty     = 0x0c
};

// Prints piece as unicode chess piece symbol.
std::ostream& operator<<(std::ostream& os, piece_idx v);

char to_fen_char(piece_idx idx);
piece_idx from_fen_char(char c);

// A to-function is only possible here, as the algebraic notation does not distinguish colours.
char to_algebraic_char(piece_idx idx);
