#include "position.hpp"
#include "board.hpp"

#include <array>

namespace 
{

constexpr const char* ANSI_RESET = "\e[0m";

constexpr const char* ANSI_BOLD_BLACK = "\e[1;30m";
constexpr const char* ANSI_BOLD_WHITE = "\e[1;37m";

constexpr const char* ANSI_BACKGROUND_BLACK = "\e[40m";
constexpr const char* ANSI_BACKGROUND_WHITE = "\e[47m";

enum piece
{
    white_pawn, white_knight, white_king, white_rook, white_queen, white_bishop,
    black_pawn, black_knight, black_king, black_rook, black_queen, black_bishop,
    empty
};

using mailbox_board = std::array<piece, 64>;

std::ostream& operator<<(std::ostream& os, piece v)
{
    switch (v)
    {
        case white_pawn:   return os << ANSI_BOLD_WHITE << "P ";
        case white_knight: return os << ANSI_BOLD_WHITE << "Kn";
        case white_king:   return os << ANSI_BOLD_WHITE << "K ";
        case white_rook:   return os << ANSI_BOLD_WHITE << "R ";
        case white_queen:  return os << ANSI_BOLD_WHITE << "Q ";
        case white_bishop: return os << ANSI_BOLD_WHITE << "B ";
        case black_pawn:   return os << ANSI_BOLD_BLACK << "P ";
        case black_knight: return os << ANSI_BOLD_BLACK << "Kn";
        case black_king:   return os << ANSI_BOLD_BLACK << "K ";
        case black_rook:   return os << ANSI_BOLD_BLACK << "R ";
        case black_queen:  return os << ANSI_BOLD_BLACK << "Q ";
        case black_bishop: return os << ANSI_BOLD_BLACK << "B ";
        default: return os << "  ";
    }
}

std::ostream& operator<<(std::ostream& os, const mailbox_board& v)
{
    os << ANSI_RESET;
    for (std::size_t rank = 0; rank < 8; rank++)
    {
        for (std::size_t file = 0; file < 8; file++)
            os << (WHITE_SQUARES & get_bitboard_mailbox_piece(rank*8+file) ? ANSI_BACKGROUND_BLACK : ANSI_BACKGROUND_WHITE) << v[(7-rank)*8+file] << ' ';
        os << ANSI_RESET << '\n';
    }
    return os;
}

mailbox_board get_mailbox_board_from_position(const position& v)
{
    mailbox_board ret;

    for (std::size_t i = 0; i < ret.size(); i++)
    {
        const bitboard b { 1ULL << i };

        if (b & v.white_pieces & v.pawns)
            ret[i] = white_pawn;
        else if (b & v.white_pieces & v.knights)
            ret[i] = white_knight;
        else if (b & v.white_pieces & v.kings)
            ret[i] = white_king;
        else if (b & v.white_pieces & v.queens)
            ret[i] = white_queen;
        else if (b & v.white_pieces & v.rooks)
            ret[i] = white_rook;
        else if (b & v.white_pieces & v.bishops)
            ret[i] = white_bishop;
        else if (b & v.black_pieces & v.pawns)
            ret[i] = black_pawn;
        else if (b & v.black_pieces & v.knights)
            ret[i] = black_knight;
        else if (b & v.black_pieces & v.kings)
            ret[i] = black_king;
        else if (b & v.black_pieces & v.queens)
            ret[i] = black_queen;
        else if (b & v.black_pieces & v.rooks)
            ret[i] = black_rook;
        else if (b & v.black_pieces & v.bishops)
            ret[i] = black_bishop;
        else
            ret[i] = empty;
    }

    return ret;
}

}

std::ostream& operator<<(std::ostream& os, const position& v)
{
    return os << get_mailbox_board_from_position(v);
}