#include "position.hpp"
#include "bitboard.hpp"

#include "pieces/pieces.hpp"

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
            os << (WHITE_SQUARES & get_bitboard_mailbox_piece(rank*8+file) ? ANSI_BACKGROUND_BLACK : ANSI_BACKGROUND_WHITE) << v[(7-rank)*8+file];
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

void get_white_pawn_moves(position current_position, std::vector<position>& next_positions)
{
    position next_position = current_position;
    next_position.white_to_move     = false;
    next_position.en_passent_square = 0;

    for (bitboard white_pawns { current_position.white_pieces & current_position.pawns }; white_pawns; )
    {
        const bitboard pawn { ls1b_isolate(white_pawns) };

        // Calcualte attacking moves.
        {
            const bitboard attacked_squares { get_white_pawn_all_attacked_squares_from_bitboard(pawn) };

            // Regular attacks.
            for (bitboard moves { attacked_squares & current_position.black_pieces }; moves; )
            {
                const bitboard move { ls1b_isolate(moves) };
                
                {
                    next_position.pawns        = (current_position.pawns        ^ pawn) | move;
                    next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
                    next_position.black_pieces = current_position.black_pieces  ^ move;

                    next_positions.push_back(next_position);
                }

                moves ^= move;
            }

            // En-passent attack. We don't have to loop here as there is at most one en-passent capture square in the position.
            if (const bitboard move { attacked_squares & current_position.en_passent_square }; move)
            {
                next_position.pawns        = (current_position.pawns        ^ pawn) | move;
                next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
                next_position.black_pieces = current_position.black_pieces  ^ current_position.en_passent_piece;

                next_positions.push_back(next_position);
            }
        }

        // Calculate single-push moves. There can be at most one of these.
        if (const bitboard move { get_white_pawn_single_push_squares_from_bitboard(pawn, ~(current_position.white_pieces | current_position.black_pieces)) }; move)
        {
            next_position.pawns        = (current_position.pawns        ^ pawn) | move;
            next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
            
            next_positions.push_back(next_position);
        }

        // Calculate double-push moves and remember to set the en-passent data (and clear it after). It is important to do this first to clear the en-passent bit.
        if (const bitboard move { get_white_pawn_double_push_squares_from_bitboard(pawn, ~(current_position.white_pieces | current_position.black_pieces)) }; move)
        {
            next_position.pawns             = (current_position.pawns        ^ pawn) | move;
            next_position.white_pieces      = (current_position.white_pieces ^ pawn) | move;
            next_position.en_passent_piece  = move;
            next_position.en_passent_square = pawn << 8;
            
            next_positions.push_back(next_position);
            next_position.en_passent_square = 0;
        }

        // TODO: Handle promotion.

        // Mask off this bit before looping again.
        white_pawns ^= pawn;
    }
}

void get_knight_moves(position current_position, std::vector<position>& next_positions)
{
    position next_position = current_position;
    next_position.white_to_move = !current_position.white_to_move;
    next_position.en_passent_square = 0;
    
    for (bitboard to_move_knights { to_move_pieces(current_position) & current_position.knights }; to_move_knights; )
    {
        const bitboard knight_bitboard { ls1b_isolate(to_move_knights) };
        const mailbox knight_mailbox { bitscan_forward( to_move_knights) };

        for (bitboard moves { get_knight_attacked_squares_from_mailbox(knight_mailbox) & ~to_move_pieces(current_position) }; moves; )
        {
            const bitboard move { ls1b_isolate(moves) };
            
            {
                next_position.knights = (current_position.knights ^ knight_bitboard) | move;
                opponent_pieces(next_position) = (to_move_pieces(current_position) ^ knight_bitboard) | move;
                to_move_pieces(next_position)  = opponent_pieces(current_position) & ~move;

                next_positions.push_back(next_position);
            }

            moves ^= move;
        }

        to_move_knights ^= knight_bitboard;
    }
}

std::vector<position> get_next_positions(position pos)
{
    std::vector<position> ret;

    if (pos.white_to_move)
        get_white_pawn_moves(pos, ret);

    get_knight_moves(pos, ret);

    return ret;
}