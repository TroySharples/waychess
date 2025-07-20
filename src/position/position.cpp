#include "position.hpp"
#include "pieces/king.hpp"
#include "pieces/knight.hpp"
#include "pieces/pawn.hpp"
#include "pieces/pieces.hpp"
#include "pieces/rook.hpp"

std::uint64_t get_attackers(const bitboard& bb, std::uint8_t mb)
{
    const std::uint64_t pieces_bb  { bb.w_pieces | bb.b_pieces };

    return (get_king_attacked_squares_from_mailbox(mb)              & (bb.boards[piece_idx::w_king]   | bb.boards[piece_idx::b_king]))
         | (get_knight_attacked_squares_from_mailbox(mb)            & (bb.boards[piece_idx::w_knight] | bb.boards[piece_idx::b_knight]))
         | (get_rook_attacked_squares_from_mailbox(mb, pieces_bb)   & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_queen]))
         | (get_bishop_attacked_squares_from_mailbox(mb, pieces_bb) & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_queen]))
         | (get_white_pawn_all_attacked_squares_from_mailbox(mb)    & bb.boards[piece_idx::b_pawn])
         | (get_black_pawn_all_attacked_squares_from_mailbox(mb)    & bb.boards[piece_idx::w_pawn]);
}

std::uint64_t get_attackers_white(const bitboard& bb, std::uint8_t mb)
{
    const std::uint64_t pieces_bb  { bb.w_pieces | bb.b_pieces };

    return (get_king_attacked_squares_from_mailbox(mb)              & bb.boards[piece_idx::w_king])
         | (get_knight_attacked_squares_from_mailbox(mb)            & bb.boards[piece_idx::w_knight])
         | (get_rook_attacked_squares_from_mailbox(mb, pieces_bb)   & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::w_queen]))
         | (get_bishop_attacked_squares_from_mailbox(mb, pieces_bb) & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::w_queen]))
         | (get_black_pawn_all_attacked_squares_from_mailbox(mb)    & bb.boards[piece_idx::w_pawn]);
}

std::uint64_t get_attackers_black(const bitboard& bb, std::uint8_t mb)
{
    const std::uint64_t pieces_bb  { bb.w_pieces | bb.b_pieces };

    return (get_king_attacked_squares_from_mailbox(mb)              & bb.boards[piece_idx::b_king])
         | (get_knight_attacked_squares_from_mailbox(mb)            & bb.boards[piece_idx::b_knight])
         | (get_rook_attacked_squares_from_mailbox(mb, pieces_bb)   & (bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::b_queen]))
         | (get_bishop_attacked_squares_from_mailbox(mb, pieces_bb) & (bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::b_queen]))
         | (get_white_pawn_all_attacked_squares_from_mailbox(mb)    & bb.boards[piece_idx::b_pawn]);
}