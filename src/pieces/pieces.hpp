#pragma once

#include "king.hpp"
#include "pawn.hpp"
#include "knight.hpp"
#include "rook.hpp"
#include "bishop.hpp"
#include "queen.hpp"

// Enumerates our piece types. This is also used to index our array of bitboards, which includes
// the all-white / all-black bitboards. We also have an empty piece idx (even though we don't
// maintain an empty bitboard), which is convenient in mailbox representations. Note that the
// first three bits of each piece type match for each side, which helps in some comparison logic.
enum piece_idx : std::uint8_t
{
    w_pawn   = 000,
    w_king   = 001,
    w_knight = 002,
    w_bishop = 003,
    w_rook   = 004,
    w_queen  = 005,
    w_any    = 006,
    b_any    = 007,
    b_pawn   = 010,
    b_king   = 011,
    b_knight = 012,
    b_bishop = 013,
    b_rook   = 014,
    b_queen  = 015,
    empty    = 016
};

// Prints piece as unicode chess piece symbol.
std::ostream& operator<<(std::ostream& os, piece_idx v);

char to_fen_char(piece_idx idx);
piece_idx from_fen_char(char c);

// A to-function is only possible here, as the algebraic notation does not distinguish colours.
char to_algebraic_char(piece_idx idx);
