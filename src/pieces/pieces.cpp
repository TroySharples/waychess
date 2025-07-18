#include "pieces.hpp"

#include <stdexcept>

std::ostream& operator<<(std::ostream& os, piece_idx v)
{
    switch (v)
    {
        case w_pawn:   return os << "\u2659 ";
        case w_knight: return os << "\u2658 ";
        case w_king:   return os << "\u2654 ";
        case w_rook:   return os << "\u2656 ";
        case w_queen:  return os << "\u2655 ";
        case w_bishop: return os << "\u2657 ";
        case b_pawn:   return os << "\u265f ";
        case b_knight: return os << "\u265e ";
        case b_king:   return os << "\u265a ";
        case b_rook:   return os << "\u265c ";
        case b_queen:  return os << "\u265b ";
        case b_bishop: return os << "\u265d ";
        case empty:    return os << "  ";
        default: throw std::invalid_argument("Unknown piece-idx " + std::to_string(static_cast<int>(v)));
    }
}

char to_fen_char(piece_idx idx)
{
    switch (idx)
    {
        case w_pawn:   return 'P';
        case w_knight: return 'N';
        case w_king:   return 'K';
        case w_rook:   return 'R';
        case w_queen:  return 'Q';
        case w_bishop: return 'B';
        case b_pawn:   return 'p';
        case b_knight: return 'n';
        case b_king:   return 'k';
        case b_rook:   return 'r';
        case b_queen:  return 'q';
        case b_bishop: return 'b';
        default: throw std::invalid_argument("Cannot convert piece-idx " + std::to_string(static_cast<int>(idx)) + " to fen char");
    }
}

piece_idx from_fen_char(char c)
{
    switch (c)
    {
        case 'P': return w_pawn;
        case 'N': return w_knight;
        case 'K': return w_king;
        case 'R': return w_rook;
        case 'Q': return w_queen;
        case 'B': return w_bishop;
        case 'p': return b_pawn;
        case 'n': return b_knight;
        case 'k': return b_king;
        case 'r': return b_rook;
        case 'q': return b_queen;
        case 'b': return b_bishop;
        default: throw std::invalid_argument("Unknown fen char " + std::to_string(c));
    }
}

char to_algebraic_char(piece_idx idx)
{
    switch (idx)
    {
        case w_pawn:
        case b_pawn:   throw std::invalid_argument("Pawns are not represented by a char in algebraic notation");
        case w_knight:
        case b_knight: return 'N';
        case w_king:
        case b_king:   return 'K';
        case w_rook:
        case b_rook:   return 'R';
        case w_queen:
        case b_queen:  return 'Q';
        case w_bishop:
        case b_bishop: return 'B';
        default: throw std::invalid_argument("Cannot convert piece-idx " + std::to_string(static_cast<int>(idx)) + " to algebraic char");
    }
}