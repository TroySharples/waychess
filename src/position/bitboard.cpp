#include "bitboard.hpp"

#include "mailbox.hpp"

bitboard::bitboard(const mailbox& mb)
    : en_passent_bb(mb.en_passent_square.has_value() ? get_bitboard_mailbox_piece(*mb.en_passent_square) : 0), castling(mb.castling), ply_counter(mb.ply_counter), ply_50m(mb.ply_50m)
{
    boards.fill(0);
    for (std::size_t i = 0; i < mb.squares.size(); i++)
        if (const auto p = mb.squares[i]; p != piece_idx::empty) 
            boards[p] |= get_bitboard_mailbox_piece(i);

    boards[piece_idx::w_any] = 0;
    for (std::underlying_type_t<piece_idx> p = w_pawn; p <= w_queen; p++)
        boards[piece_idx::w_any] |= boards[p];
    boards[piece_idx::b_any] = 0;
    for (std::underlying_type_t<piece_idx> p = b_pawn; p <= b_queen; p++)
        boards[piece_idx::b_any] |= boards[p];
}

bitboard::bitboard(std::string_view fen)
    : bitboard(mailbox(fen))
{
    
}

std::string bitboard::get_fen_string() const noexcept
{
    return mailbox(*this).get_fen_string();
}

std::ostream& bitboard::display_unicode_board(std::ostream& os, bool flipped) const noexcept
{
    return mailbox(*this).display_unicode_board(os, flipped);
}
