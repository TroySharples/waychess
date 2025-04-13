#include "position.hpp"
#include "board.hpp"

#include <array>

namespace 
{

constexpr const char* FG_RED        = "\033[31m";
constexpr const char* FG_GREEN      = "\033[32m";
constexpr const char* FG_BLUE       = "\033[34m";
constexpr const char* FG_LIGHT_GREY = "\033[37m";
constexpr const char* FG_DEFAULT    = "\033[39m";
constexpr const char* FG_DARK_GREY  = "\033[49m";
constexpr const char* BG_RED        = "\033[41m";
constexpr const char* BG_GREEN      = "\033[42m";
constexpr const char* BG_BLUE       = "\033[44m";
constexpr const char* BG_DEFAULT    = "\033[49m";

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
        case white_pawn:   return os << FG_LIGHT_GREY << "P ";
        case white_knight: return os << FG_LIGHT_GREY << "Kn";
        case white_king:   return os << FG_LIGHT_GREY << "K ";
        case white_rook:   return os << FG_LIGHT_GREY << "R ";
        case white_queen:  return os << FG_LIGHT_GREY << "Q ";
        case white_bishop: return os << FG_LIGHT_GREY << "B ";
        case black_pawn:   return os << FG_DARK_GREY  << "P ";
        case black_knight: return os << FG_DARK_GREY  << "Kn";
        case black_king:   return os << FG_DARK_GREY  << "K ";
        case black_rook:   return os << FG_DARK_GREY  << "R ";
        case black_queen:  return os << FG_DARK_GREY  << "Q ";
        case black_bishop: return os << FG_DARK_GREY  << "B ";
        default: return os << "  ";
    }
}

std::ostream& operator<<(std::ostream& os, const mailbox_board& v)
{
    for (std::size_t rank = 0; rank < 8; rank++)
    {
        for (std::size_t file = 0; file < 8; file++)
            os << v[(7-rank)*8+file] << ' ';
        os << '\n';
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