#pragma once

#include "bitboard.hpp"
#include "pieces/pieces.hpp"

inline std::uint64_t get_xrayers(const bitboard& bb, std::size_t mb) noexcept
{
    return (get_rook_xrayed_squares_from_mailbox(mb)   & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_queen]))
         | (get_bishop_xrayed_squares_from_mailbox(mb) & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_queen]));
}