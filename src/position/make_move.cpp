#include "make_move.hpp"

#include "move.hpp"
#include "pieces/pieces.hpp"

void make_move(bitboard& bb, std::uint32_t move)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces  { is_black_to_play ? bb.b_pieces : bb.w_pieces };

    const std::uint8_t from_mb { get_from_square(move) };
    const std::uint8_t to_mb { get_to_square(move) };
    const piece_idx piece { get_piece_idx(move) };
    const std::uint16_t type { get_move_type(move) };

    const std::uint64_t from_bb { 1ULL << from_mb };
    const std::uint64_t to_bb   { 1ULL << to_mb };
    const std::uint64_t from_to_bb { from_bb | to_bb };

    // Increment the ply counter and reset the en passent square.
    bb.en_passent_square = 0;
    bb.ply_counter++;

    // Handle mechanically moving the side-to-plays piece. We have to be careful about the special case of
    // pawn promotions when updating the piece-specific bitboard.
    to_move_pieces ^= from_to_bb;
    if (type & move_type::PROMOTION)
    {
        bb.boards[piece] ^= from_bb;
        if (type & move_type::PROMOTION_QUEEN)
            bb.boards[is_black_to_play ? piece_idx::b_queen : piece_idx::w_queen] ^= to_bb;
        else if (type & move_type::PROMOTION_KNIGHT)
            bb.boards[is_black_to_play ? piece_idx::b_knight : piece_idx::w_knight] ^= to_bb;
        else if (type & move_type::PROMOTION_ROOK)
            bb.boards[is_black_to_play ? piece_idx::b_rook : piece_idx::w_rook] ^= to_bb;
        else if (type & move_type::PROMOTION_BISHOP)
            bb.boards[is_black_to_play ? piece_idx::b_bishop : piece_idx::w_bishop] ^= to_bb;
    }
    else
    {
        bb.boards[piece] ^= from_to_bb;
    }

    // Handle piece captures.
    if (type & move_type::CAPTURE)
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };

        // En passent captures are special - the piece to be taken off the board isn't the target move square.
        if (type & move_type::EN_PASSENT_CAPTURE)
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
    // If the move was a double pawn push we have to update the en passent target square for the next move.
    else if (type & move_type::DOUBLE_PAWN_PUSH)
    {
        bb.en_passent_square = (is_black_to_play ? to_bb : from_bb) << 8;

        bb.ply_50m = 0;
    }
    // If the move was just a single pawn push we still need to reset the ply counter for the 50 move rule.
    else if (type & move_type::SINGLE_PAWN_PUSH)
    {
        bb.ply_50m = 0;
    }
    // Otherwise we can increment the counter towards the 50 move limit.
    else
    {
        bb.ply_50m++;
    }
}