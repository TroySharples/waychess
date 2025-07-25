#include "make_move.hpp"

#include "move.hpp"
#include "pieces/pieces.hpp"
#include "position.hpp"
#include "position/generate_moves.hpp"

namespace
{

bool make_move_impl(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint8_t& capture_idx) noexcept
{
    bool ret { true };

    const bool is_black_to_play   { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };

    // The move only contains the piece up to colour - exactly which colour piece must be derived from the side-to-play.
    const std::uint8_t from_mb { move::deserialise_from_mb(move) };
    const std::uint8_t to_mb   { move::deserialise_to_mb(move) };
    const piece_idx piece      { static_cast<piece_idx>(move::deserialise_piece_idx(move) | (is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn)) };
    const std::uint8_t type    { move::deserialise_move_type(move) };
    const std::uint8_t info    { move::deserialise_move_info(move) };

    const std::uint64_t from_bb    { 1ULL << from_mb };
    const std::uint64_t to_bb      { 1ULL << to_mb };
    const std::uint64_t from_to_bb { from_bb | to_bb };

    // Increment the ply counter and reset the en-passent square.
    bb.en_passent_bb = 0;
    bb.ply_counter++;

    // Handle the removal of castling rights. We use the from-to bitboard instead of just the from one as we also have
    // to remember to remove castling rights in the event one of our rooks gets captured - this is actually quite subtle...
    if (from_to_bb & ((FILE_E & RANK_1) | (FILE_A & RANK_1))) bb.castling &= ~bitboard::CASTLING_W_QS;
    if (from_to_bb & ((FILE_E & RANK_1) | (FILE_H & RANK_1))) bb.castling &= ~bitboard::CASTLING_W_KS;
    if (from_to_bb & ((FILE_E & RANK_8) | (FILE_A & RANK_8))) bb.castling &= ~bitboard::CASTLING_B_QS;
    if (from_to_bb & ((FILE_E & RANK_8) | (FILE_H & RANK_8))) bb.castling &= ~bitboard::CASTLING_B_KS;

    // Next we handle mechanically moving the side-to-plays piece. We have to be careful about the special case of
    // pawn promotions when updating the piece-specific bitboard.
    to_move_pieces ^= from_to_bb;
    if (move::move_type::is_promotion(type))
    {
        bb.boards[piece] ^= from_bb;
        bb.boards[info | (is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn)] ^= to_bb;
    }
    else
    {
        bb.boards[piece] ^= from_to_bb;
    }

    // Handle piece captures.
    capture_idx = 0;
    if (move::move_type::is_capture(type))
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };

        // En-passent captures are special - the piece to be taken off the board isn't the target move square.
        if (move::move_type::is_en_passent(type))
        {
            const std::uint64_t capture_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);

            opponent_pieces ^= capture_bb;
            bb.boards[is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn] ^= capture_bb;
        }
        // For regular piece captures, we have to loop through the opponents piece bitboards to find the one containing
        // the piece to remove. We also have to remember to fill in the unmake-move so we can recover the piece when
        // going in reverse.
        else
        {
            opponent_pieces ^= to_bb;
            for (capture_idx = is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn; capture_idx <= (is_black_to_play ? piece_idx::w_queen : piece_idx::b_queen); capture_idx++)
            {
                if (std::uint64_t& capture_bitboard = bb.boards[capture_idx]; capture_bitboard & to_bb)
                {
                    capture_bitboard ^= to_bb;
                    break;
                }
            }
            capture_idx |= 0x10;
        }

        bb.ply_50m = 0;
    }
    // If the move was just a pawn push we need to reset the ply counter for the 50 move rule.
    else if (move::move_type::is_pawn_push(type))
    {
        bb.ply_50m = 0;

        // If the move was a double pawn push we have to update the en-passent target square for the next move.
        if (info == move::move_info::PAWN_PUSH_DOUBLE)
            bb.en_passent_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);
    }
    // Otherwise we can increment the counter towards the 50 move limit.
    else
    {
        bb.ply_50m++;
    }

    // Handle moving the rook for castling (legality is checked below).
    if (move::move_type::is_castle(type))
    {
        if (info == move::move_info::CASTLE_KS)
        {
            if (is_black_to_play)
            {
                constexpr std::uint64_t from_to_bb { (FILE_H & RANK_8) | (FILE_F & RANK_8) };
                bb.boards[piece_idx::b_rook] ^= from_to_bb;
                bb.boards[piece_idx::b_any]  ^= from_to_bb;
            }
            else
            {
                constexpr std::uint64_t from_to_bb { (FILE_H & RANK_1) | (FILE_F & RANK_1) };
                bb.boards[piece_idx::w_rook] ^= from_to_bb;
                bb.boards[piece_idx::w_any]  ^= from_to_bb;
            }
        }
        else
        {
            if (is_black_to_play)
            {
                constexpr std::uint64_t from_to_bb { (FILE_A & RANK_8) | (FILE_D & RANK_8) };
                bb.boards[piece_idx::b_rook] ^= from_to_bb;
                bb.boards[piece_idx::b_any]  ^= from_to_bb;
            }
            else
            {
                constexpr std::uint64_t from_to_bb { (FILE_A & RANK_1) | (FILE_D & RANK_1) };
                bb.boards[piece_idx::w_rook] ^= from_to_bb;
                bb.boards[piece_idx::w_any]  ^= from_to_bb;
            }
        }
    }

    // Handle left-in-check legality checking.
    if (args.check_legality)
    {
        if (move::move_type::is_castle(type))
        {
            if (is_black_to_play)
            {
                if (info == move::move_info::CASTLE_KS)
                    ret = !get_attackers_white(bb, std::countr_zero(FILE_E & RANK_8), std::countr_zero(FILE_F & RANK_8), std::countr_zero(FILE_G & RANK_8));
                else
                    ret = !get_attackers_white(bb, std::countr_zero(FILE_E & RANK_8), std::countr_zero(FILE_D & RANK_8), std::countr_zero(FILE_C & RANK_8));
            }
            else
            {
                if (info == move::move_info::CASTLE_KS)
                    ret = !get_attackers_black(bb, std::countr_zero(FILE_E & RANK_1), std::countr_zero(FILE_F & RANK_1), std::countr_zero(FILE_G & RANK_1));
                else
                    ret = !get_attackers_black(bb, std::countr_zero(FILE_E & RANK_1), std::countr_zero(FILE_D & RANK_1), std::countr_zero(FILE_C & RANK_1));
            }
        }
        else
        {
            if (is_black_to_play)
            {
                ret = !get_attackers_white(bb, std::countr_zero(bb.boards[piece_idx::b_king]));
            }
            else
            {
                ret = !get_attackers_black(bb, std::countr_zero(bb.boards[piece_idx::w_king]));
            }
        }
    }

    return ret;
}

}

bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake) noexcept
{
    // The first 18 bits of the unmake are the same as the forward move.
    unmake = move & 0x3ffff;

    std::uint8_t captured_piece;
    const bool ret { make_move_impl(args, bb, move, captured_piece) };

    // TODO: fill out the inverse move parameters.

    return ret;

}

bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move) noexcept
{
    std::uint8_t captured_piece;
    return make_move_impl(args, bb, move, captured_piece);
}