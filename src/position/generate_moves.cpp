#include "generate_moves.hpp"

#include "move.hpp"
#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"
#include "utility/binary.hpp"

// void get_white_pawn_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
// {
//     position next_position = current_position;
//     next_position.white_to_move     = false;
//     next_position.en_passent_square = 0;

//     for (bitboard white_pawns { current_position.white_pieces & current_position.pawns }; white_pawns; )
//     {
//         const bitboard pawn { ls1b_isolate(white_pawns) };

//         // Calcualte attacking moves.
//         {
//             const bitboard attacked_squares { get_white_pawn_all_attacked_squares_from_bitboard(pawn) };

//             // Regular attacks.
//             for (bitboard moves { attacked_squares & current_position.black_pieces }; moves; )
//             {
//                 const bitboard move { ls1b_isolate(moves) };
                
//                 {
//                     next_position.pawns        = (current_position.pawns        ^ pawn) | move;
//                     next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
//                     next_position.black_pieces = current_position.black_pieces  ^ move;

//                     next_positions.push_back(next_position);
//                 }

//                 moves ^= move;
//             }

//             // En-passent attack. We don't have to loop here as there is at most one en-passent capture square in the position.
//             if (const bitboard move { attacked_squares & current_position.en_passent_square }; move)
//             {
//                 next_position.pawns        = (current_position.pawns        ^ pawn) | move;
//                 next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
//                 next_position.black_pieces = current_position.black_pieces  ^ current_position.en_passent_piece;

//                 next_positions.push_back(next_position);
//             }
//         }

//         // Calculate single-push moves. There can be at most one of these.
//         if (const bitboard move { get_white_pawn_single_push_squares_from_bitboard(pawn, ~(current_position.white_pieces | current_position.black_pieces)) }; move)
//         {
//             next_position.pawns        = (current_position.pawns        ^ pawn) | move;
//             next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
            
//             next_positions.push_back(next_position);
//         }

//         // Calculate double-push moves and remember to set the en-passent data (and clear it after). It is important to do this first to clear the en-passent bit.
//         if (const bitboard move { get_white_pawn_double_push_squares_from_bitboard(pawn, ~(current_position.white_pieces | current_position.black_pieces)) }; move)
//         {
//             next_position.pawns             = (current_position.pawns        ^ pawn) | move;
//             next_position.white_pieces      = (current_position.white_pieces ^ pawn) | move;
//             next_position.en_passent_piece  = move;
//             next_position.en_passent_square = pawn << 8;
            
//             next_positions.push_back(next_position);
//             next_position.en_passent_square = 0;
//         }

//         // TODO: Handle promotion.

//         // Mask off this bit before looping again.
//         white_pawns ^= pawn;
//     }
// }

// void get_black_pawn_moves(position current_position, std::vector<position>& next_positions)
// {

// }

void get_knight_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const piece_idx to_move_idx { bb.is_black_to_play() ? piece_idx::b_knight : piece_idx::w_knight };
    const std::uint64_t to_move_pieces { bb.is_black_to_play() ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { bb.is_black_to_play() ? bb.w_pieces : bb.b_pieces };

    const std::uint32_t piece_move { serialise_move(0, 0, to_move_idx, 0) };
    for (std::uint64_t to_move_knights { bb.boards[to_move_idx] }; to_move_knights; )
    {
        const std::uint64_t knight_bitboard { ls1b_isolate(to_move_knights) };
        const std::uint8_t knight_mailbox = std::countr_zero(knight_bitboard);
        const std::uint32_t from_move { set_from_square(piece_move, knight_mailbox) };

        for (std::uint64_t attacks { get_knight_attacked_squares_from_mailbox(knight_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };
            
            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(attack)) };
                if (attack & opponent_pieces) 
                    move = set_move_type(move, move_type::CAPTURE);

                moves.push_back(set_move_type(move, attack & opponent_pieces ? move_type::CAPTURE : 0));
            }

            attacks ^= attack;
        }

        to_move_knights ^= knight_bitboard;
    }
}

// void get_rook_moves(position current_position, std::vector<position>& next_positions)
// {
//     position next_position = current_position;
//     next_position.white_to_move = !current_position.white_to_move;
//     next_position.en_passent_square = 0;
    
//     for (bitboard to_move_rooks { to_move_pieces(current_position) & current_position.rooks }; to_move_rooks; )
//     {
//         const bitboard rook_bitboard { ls1b_isolate(to_move_rooks) };
//         const mailbox rook_mailbox { bitscan_forward( to_move_rooks) };

//         for (bitboard moves { get_rook_attacked_squares_from_mailbox(rook_mailbox, current_position.white_pieces | current_position.black_pieces) & ~to_move_pieces(current_position) }; moves; )
//         {
//             const bitboard move { ls1b_isolate(moves) };
            
//             {
//                 next_position.rooks = (current_position.rooks ^ rook_bitboard) | move;
//                 opponent_pieces(next_position) = (to_move_pieces(current_position) ^ rook_bitboard) | move;
//                 to_move_pieces(next_position)  = opponent_pieces(current_position) & ~move;

//                 next_positions.push_back(next_position);
//             }

//             moves ^= move;
//         }

//         to_move_rooks ^= rook_bitboard;
//     }
// }

std::vector<std::uint32_t> generate_pseudo_legal_moves(const bitboard& bb)
{
    std::vector<std::uint32_t> ret;

    // pos.white_to_move ? get_white_pawn_moves(pos, ret) : get_black_pawn_moves(pos, ret);

    get_knight_moves(bb, ret);
    // get_rook_moves(pos, ret);

    return ret;
}