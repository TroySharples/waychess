#include "make_move.hpp"

#include "move.hpp"
#include "pieces/pieces.hpp"
#include "position.hpp"
#include "position/generate_moves.hpp"
#include <bit>

bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move)
{
    bool ret { true };

    const bool is_black_to_play   { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };

    const std::uint8_t from_mb { move::deserialise_from_mb(move) };
    const std::uint8_t to_mb   { move::deserialise_to_mb(move) };
    const piece_idx piece      { move::deserialise_piece_idx(move) };
    const std::uint16_t type   { move::deserialise_type(move) };

    const std::uint64_t from_bb    { 1ULL << from_mb };
    const std::uint64_t to_bb      { 1ULL << to_mb };
    const std::uint64_t from_to_bb { from_bb | to_bb };

    // Increment the ply counter and reset the en-passent square.
    bb.en_passent_mb = 0;
    bb.ply_counter++;

    // We first check the legality of castling from and through check. It's clearest (I haven't
    // decided if it's necessary) to do this first before any pieces have moved as not to influence
    // the attacked-square calculation. This comes at the expense of duplicating the code tree
    // structure of the rook-moving logic, but oh well.
    if (args.check_legality && type & move::move_type::CASTLE)
    {
        // The stratagy here is to keep track of the bitboard of attackers attacking the from and
        // through castling squares attacked by the relevant side.
        std::uint64_t attacked_squares {};

        if (type & move::move_type::CASTLE_KS)
        {
            if (is_black_to_play)
            {
                attacked_squares |= get_attackers_white(bb, std::countr_zero(FILE_E & RANK_8))
                                  | get_attackers_white(bb, std::countr_zero(FILE_F & RANK_8));
            }
            else
            {
                attacked_squares |= get_attackers_black(bb, std::countr_zero(FILE_E & RANK_1))
                                  | get_attackers_black(bb, std::countr_zero(FILE_F & RANK_1));
            }
        }
        else
        {
            if (is_black_to_play)
            {
                attacked_squares |= get_attackers_white(bb, std::countr_zero(FILE_E & RANK_8))
                                  | get_attackers_white(bb, std::countr_zero(FILE_D & RANK_8))
                                  | get_attackers_white(bb, std::countr_zero(FILE_C & RANK_8));
            }
            else
            {
                attacked_squares |= get_attackers_black(bb, std::countr_zero(FILE_E & RANK_1))
                                  | get_attackers_black(bb, std::countr_zero(FILE_D & RANK_1))
                                  | get_attackers_black(bb, std::countr_zero(FILE_C & RANK_1));
            }
        }

        if (attacked_squares)
            ret = false;
    }

    // Next we handle mechanically moving the side-to-plays piece. We have to be careful about the special case of
    // pawn promotions when updating the piece-specific bitboard.
    to_move_pieces ^= from_to_bb;
    if (type & move::move_type::PROMOTION)
    {
        bb.boards[piece] ^= from_bb;
        if (type & move::move_type::PROMOTION_QUEEN)
            bb.boards[is_black_to_play ? piece_idx::b_queen : piece_idx::w_queen] ^= to_bb;
        else if (type & move::move_type::PROMOTION_KNIGHT)
            bb.boards[is_black_to_play ? piece_idx::b_knight : piece_idx::w_knight] ^= to_bb;
        else if (type & move::move_type::PROMOTION_ROOK)
            bb.boards[is_black_to_play ? piece_idx::b_rook : piece_idx::w_rook] ^= to_bb;
        else if (type & move::move_type::PROMOTION_BISHOP)
            bb.boards[is_black_to_play ? piece_idx::b_bishop : piece_idx::w_bishop] ^= to_bb;
    }
    else
    {
        bb.boards[piece] ^= from_to_bb;
    }

    // Handle piece captures.
    if (type & move::move_type::CAPTURE)
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };

        // En-passent captures are special - the piece to be taken off the board isn't the target move square.
        if (type & move::move_type::EN_PASSENT_CAPTURE)
        {
            const std::uint64_t capture_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);

            opponent_pieces ^= capture_bb;
            bb.boards[is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn] ^= capture_bb;
        }
        // For regular piece captures, we have to loop through the opponents piece bitboards to find the one containing
        // the piece to remove.
        else
        {
            opponent_pieces ^= to_bb;
            for (std::uint8_t idx = is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn; idx <= is_black_to_play ? piece_idx::w_queen : piece_idx::b_queen; idx++)
            {
                if (std::uint64_t& capture_bitboard = bb.boards[idx]; capture_bitboard & to_bb)
                {
                    capture_bitboard ^= to_bb;
                    break;
                }
            }
        }

        bb.ply_50m = 0;
    }
    // If the move was a double pawn push we have to update the en-passent target square for the next move.
    else if (type & move::move_type::DOUBLE_PAWN_PUSH)
    {
        bb.en_passent_mb = (is_black_to_play ? to_bb : from_bb) << 8;

        bb.ply_50m = 0;
    }
    // If the move was just a single pawn push we still need to reset the ply counter for the 50 move rule.
    else if (type & move::move_type::SINGLE_PAWN_PUSH)
    {
        bb.ply_50m = 0;
    }
    // Otherwise we can increment the counter towards the 50 move limit.
    else
    {
        bb.ply_50m++;
    }

    // Handle moving the rook for castling (legality is checked above).
    if (type & move::move_type::CASTLE)
    {
        if (type & move::move_type::CASTLE_KS)
        {
            if (is_black_to_play)
            {
                bb.boards[piece_idx::b_rook] ^= (FILE_H & RANK_8) | (FILE_F & RANK_8);
            }
            else
            {
                bb.boards[piece_idx::w_rook] ^= (FILE_H & RANK_1) | (FILE_F & RANK_1);
            }
        }
        else
        {
            if (is_black_to_play)
            {
                bb.boards[piece_idx::b_rook] ^= (FILE_A & RANK_8) | (FILE_D & RANK_8);
            }
            else
            {
                bb.boards[piece_idx::w_rook] ^= (FILE_A & RANK_1) | (FILE_D & RANK_1);
            }
        }
    }

    // Handle left-in-check legality checking.
    if (args.check_legality && (
            (is_black_to_play  && get_attackers_white(bb, std::countr_zero(bb.boards[piece_idx::b_king])))
         || (!is_black_to_play && get_attackers_black(bb, std::countr_zero(bb.boards[piece_idx::w_king])))
        ))
        ret = false;


    // Handle the removal of castling rights.
    switch (piece)
    {
        case piece_idx::w_king:
            bb.castling &= 0b1100;
            break;
        case piece_idx::b_king:
            bb.castling &= 0b0011;
            break;
        case piece_idx::w_rook:
            if (!(bb.boards[w_rook] & FILE_A & RANK_1)) bb.castling &= 0b1101;
            if (!(bb.boards[w_rook] & FILE_H & RANK_1)) bb.castling &= 0b1110;
            break;
        case piece_idx::b_rook:
            if (!(bb.boards[w_rook] & FILE_A & RANK_8)) bb.castling &= 0b0111;
            if (!(bb.boards[w_rook] & FILE_H & RANK_8)) bb.castling &= 0b1011;
            break;
        default:
            NULL;
    }

    return ret;
}