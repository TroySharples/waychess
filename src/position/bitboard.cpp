#include "bitboard.hpp"

#include "mailbox.hpp"

bitboard::bitboard(const mailbox& mb)
{
    boards.fill(0);
    for (std::size_t i = 0; i < mb.squares.size(); i++)
        if (const auto p = mb.squares[i]; p != piece_idx::empty) 
            boards[p] |= get_bitboard_mailbox_piece(i);

    w_pieces = 0;
    for (std::underlying_type_t<piece_idx> p = w_pawn; p <= w_queen; p++)
        w_pieces |= boards[p];
    b_pieces = 0;
    for (std::underlying_type_t<piece_idx> p = b_pawn; p <= b_queen; p++)
        b_pieces |= boards[p];

    en_passent_bb = mb.en_passent_square.has_value() ? get_bitboard_mailbox_piece(*mb.en_passent_square) : 0;

    castling = 0;
    if (mb.castling_w_ks) castling |= CASTLING_W_KS;
    if (mb.castling_w_qs) castling |= CASTLING_W_QS;
    if (mb.castling_b_ks) castling |= CASTLING_B_KS;
    if (mb.castling_b_qs) castling |= CASTLING_B_QS;

    ply_counter = mb.ply_counter;
    ply_50m     = mb.ply_50m;
}

bitboard::bitboard(std::string_view fen)
    : bitboard(mailbox(fen))
{
    
}

std::string bitboard::get_fen_string() const noexcept
{
    return mailbox(*this).get_fen_string();
}

std::ostream& operator<<(std::ostream& os, const bitboard& v)
{
    return os << mailbox(v);
}
