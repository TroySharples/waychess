#pragma once

#include "bitboard.hpp"

inline std::uint64_t get_attackers(const bitboard& bb, std::uint8_t mb) noexcept
{
    const std::uint64_t pieces_bb  { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    return (get_king_attacked_squares_from_mailbox(mb)              & (bb.boards[piece_idx::w_king]   | bb.boards[piece_idx::b_king]))
         | (get_knight_attacked_squares_from_mailbox(mb)            & (bb.boards[piece_idx::w_knight] | bb.boards[piece_idx::b_knight]))
         | (get_rook_attacked_squares_from_mailbox(pieces_bb, mb)   & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_queen]))
         | (get_bishop_attacked_squares_from_mailbox(pieces_bb, mb) & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_queen]))
         | (get_white_pawn_all_attacked_squares_from_mailbox(mb)    & bb.boards[piece_idx::b_pawn])
         | (get_black_pawn_all_attacked_squares_from_mailbox(mb)    & bb.boards[piece_idx::w_pawn]);
}

inline std::uint64_t get_attackers_white(const bitboard& bb, auto... mb) noexcept
{
    const std::uint64_t pieces_bb  { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    return (get_black_pawn_all_attacked_squares_from_mailboxes(mb...)    & bb.boards[piece_idx::w_pawn])
         | (get_king_attacked_squares_from_mailboxes(mb...)              & bb.boards[piece_idx::w_king])
         | (get_knight_attacked_squares_from_mailboxes(mb...)            & bb.boards[piece_idx::w_knight])
         | (get_bishop_attacked_squares_from_mailboxes(pieces_bb, mb...) & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::w_queen]))
         | (get_rook_attacked_squares_from_mailboxes(pieces_bb, mb...)   & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::w_queen]));
}

inline std::uint64_t get_attackers_black(const bitboard& bb, auto... mb) noexcept
{
    const std::uint64_t pieces_bb  { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    return (get_white_pawn_all_attacked_squares_from_mailboxes(mb...)    & bb.boards[piece_idx::b_pawn])
         | (get_king_attacked_squares_from_mailboxes(mb...)              & bb.boards[piece_idx::b_king])
         | (get_knight_attacked_squares_from_mailboxes(mb...)            & bb.boards[piece_idx::b_knight])
         | (get_bishop_attacked_squares_from_mailboxes(pieces_bb, mb...) & (bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::b_queen]))
         | (get_rook_attacked_squares_from_mailboxes(pieces_bb, mb...)   & (bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::b_queen]));
}

inline bool is_attacked_white(const bitboard& bb, auto... mb) noexcept
{
    const std::uint64_t pieces_bb  { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    if (get_black_pawn_all_attacked_squares_from_mailboxes(mb...)    & bb.boards[piece_idx::w_pawn])                                     [[unlikely]] return true;
    if (get_king_attacked_squares_from_mailboxes(mb...)              & bb.boards[piece_idx::w_king])                                     [[unlikely]] return true;
    if (get_knight_attacked_squares_from_mailboxes(mb...)            & bb.boards[piece_idx::w_knight])                                   [[unlikely]] return true;
    if (get_bishop_attacked_squares_from_mailboxes(pieces_bb, mb...) & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::w_queen])) [[unlikely]] return true;
    if (get_rook_attacked_squares_from_mailboxes(pieces_bb, mb...)   & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::w_queen])) [[unlikely]] return true;

    return false;
}

inline bool is_attacked_black(const bitboard& bb, auto... mb) noexcept
{
    const std::uint64_t pieces_bb  { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    if (get_white_pawn_all_attacked_squares_from_mailboxes(mb...)    & bb.boards[piece_idx::b_pawn])                                     [[unlikely]] return true;
    if (get_king_attacked_squares_from_mailboxes(mb...)              & bb.boards[piece_idx::b_king])                                     [[unlikely]] return true;
    if (get_knight_attacked_squares_from_mailboxes(mb...)            & bb.boards[piece_idx::b_knight])                                   [[unlikely]] return true;
    if (get_bishop_attacked_squares_from_mailboxes(pieces_bb, mb...) & (bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::b_queen])) [[unlikely]] return true;
    if (get_rook_attacked_squares_from_mailboxes(pieces_bb, mb...)   & (bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::b_queen])) [[unlikely]] return true;

    return false;
}

inline bool is_in_check(const bitboard& bb, bool is_black_to_play)
{
    if (is_black_to_play)
        return is_attacked_white(bb, std::countr_zero(bb.boards[piece_idx::b_king]));
    else
        return is_attacked_black(bb, std::countr_zero(bb.boards[piece_idx::w_king]));
}


