#include "generate_moves.hpp"

#include "move.hpp"
#include "pieces/pawn.hpp"

namespace 
{

void get_pawn_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    const std::uint32_t piece_move { serialise_move(0, 0, to_move_idx, 0) };
    for (std::uint64_t to_move_pawns { bb.boards[to_move_idx] }; to_move_pawns; )
    {
        const std::uint64_t pawn_bitboard { ls1b_isolate(to_move_pawns) };
        const std::uint8_t pawn_mailbox = std::countr_zero(pawn_bitboard);
        const std::uint32_t from_move { set_from_square(piece_move, pawn_mailbox) };

        // Handle attacking moves.
        for (std::uint64_t attacks { (is_black_to_play ? get_black_pawn_all_attacked_squares_from_mailbox(pawn_mailbox) : get_white_pawn_all_attacked_squares_from_mailbox(pawn_mailbox)) & (opponent_pieces | bb.en_passent_square) }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };
            
            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(attack)) };
                move = set_move_type(move, move_type::CAPTURE);

                // Test if this capture was en passent.
                if (attack & bb.en_passent_square)
                    move = set_move_type(move, move_type::EN_PASSENT_CAPTURE);

                // Test if this pawn move will result in promotion.
                if (attack & (is_black_to_play ? RANK_1 : RANK_8))
                {
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_QUEEN));
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_KNIGHT));
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_ROOK));
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_BISHOP));
                }
                else
                {
                    moves.push_back(move);
                }
            }

            attacks ^= attack;
        }

        // Handle single pawn pushes.
        for (std::uint64_t pushes { is_black_to_play ? get_black_pawn_single_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) : get_white_pawn_single_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) }; pushes; )
        {
            const std::uint64_t push { ls1b_isolate(pushes) };

            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(push)) };
                move = set_move_type(move, move_type::SINGLE_PAWN_PUSH);

                // Test if this pawn move will result in promotion.
                if (push & (is_black_to_play ? RANK_1 : RANK_8))
                {
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_QUEEN));
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_KNIGHT));
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_ROOK));
                    moves.push_back(set_move_type(move, move_type::PROMOTION | move_type::PROMOTION_BISHOP));
                }
                else
                {
                    moves.push_back(move);
                }
            }

            pushes ^= push;
        }

        // Handle double pawn pushes.
        for (std::uint64_t pushes { is_black_to_play ? get_black_pawn_double_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) : get_white_pawn_double_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) }; pushes; )
        {
            const std::uint64_t push { ls1b_isolate(pushes) };

            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(push)) };
                move = set_move_type(move, move_type::DOUBLE_PAWN_PUSH);

                moves.push_back(move);
            }

            pushes ^= push;
        }

        to_move_pawns ^= pawn_bitboard;
    }
}

void get_king_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_king : piece_idx::w_king };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };

    const std::uint32_t piece_move { serialise_move(0, 0, to_move_idx, 0) };
    for (std::uint64_t to_move_kings { bb.boards[to_move_idx] }; to_move_kings; )
    {
        const std::uint64_t king_bitboard { ls1b_isolate(to_move_kings) };
        const std::uint8_t king_mailbox = std::countr_zero(king_bitboard);
        const std::uint32_t from_move { set_from_square(piece_move, king_mailbox) };

        for (std::uint64_t attacks { get_king_attacked_squares_from_mailbox(king_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };
            
            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(attack)) };
                if (attack & opponent_pieces) 
                    move = set_move_type(move, move_type::CAPTURE);

                moves.push_back(move);
            }

            attacks ^= attack;
        }

        to_move_kings ^= king_bitboard;
    }
}

void get_knight_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_knight : piece_idx::w_knight };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };

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

                moves.push_back(move);
            }

            attacks ^= attack;
        }

        to_move_knights ^= knight_bitboard;
    }
}

void get_bishop_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_bishop : piece_idx::w_bishop };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t occupied_squares { to_move_pieces | opponent_pieces };

    const std::uint32_t piece_move { serialise_move(0, 0, to_move_idx, 0) };
    for (std::uint64_t to_move_bishops { bb.boards[to_move_idx] }; to_move_bishops; )
    {
        const std::uint64_t bishop_bitboard { ls1b_isolate(to_move_bishops) };
        const std::uint8_t bishop_mailbox = std::countr_zero(bishop_bitboard);
        const std::uint32_t from_move { set_from_square(piece_move, bishop_mailbox) };

        for (std::uint64_t attacks { get_bishop_attacked_squares_from_mailbox(bishop_mailbox, occupied_squares) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };
            
            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(attack)) };
                if (attack & opponent_pieces) 
                    move = set_move_type(move, move_type::CAPTURE);

                moves.push_back(move);
            }

            attacks ^= attack;
        }

        to_move_bishops ^= bishop_bitboard;
    }
}

void get_rook_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_rook : piece_idx::w_rook };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t occupied_squares { to_move_pieces | opponent_pieces };

    const std::uint32_t piece_move { serialise_move(0, 0, to_move_idx, 0) };
    for (std::uint64_t to_move_rooks { bb.boards[to_move_idx] }; to_move_rooks; )
    {
        const std::uint64_t rook_bitboard { ls1b_isolate(to_move_rooks) };
        const std::uint8_t rook_mailbox = std::countr_zero(rook_bitboard);
        const std::uint32_t from_move { set_from_square(piece_move, rook_mailbox) };

        for (std::uint64_t attacks { get_rook_attacked_squares_from_mailbox(rook_mailbox, occupied_squares) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };
            
            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(attack)) };
                if (attack & opponent_pieces) 
                    move = set_move_type(move, move_type::CAPTURE);

                moves.push_back(move);
            }

            attacks ^= attack;
        }

        to_move_rooks ^= rook_bitboard;
    }
}

void get_queen_moves(const bitboard& bb, std::vector<std::uint32_t>& moves)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_queen : piece_idx::w_queen };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t occupied_squares { to_move_pieces | opponent_pieces };

    const std::uint32_t piece_move { serialise_move(0, 0, to_move_idx, 0) };
    for (std::uint64_t to_move_queens { bb.boards[to_move_idx] }; to_move_queens; )
    {
        const std::uint64_t queen_bitboard { ls1b_isolate(to_move_queens) };
        const std::uint8_t queen_mailbox = std::countr_zero(queen_bitboard);
        const std::uint32_t from_move { set_from_square(piece_move, queen_mailbox) };

        for (std::uint64_t attacks { get_queen_attacked_squares_from_mailbox(queen_mailbox, occupied_squares) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };
            
            {
                std::uint64_t move { set_to_square(from_move, std::countr_zero(attack)) };
                if (attack & opponent_pieces) 
                    move = set_move_type(move, move_type::CAPTURE);

                moves.push_back(move);
            }

            attacks ^= attack;
        }

        to_move_queens ^= queen_bitboard;
    }
}

}

std::vector<std::uint32_t> generate_pseudo_legal_moves(const bitboard& bb)
{
    std::vector<std::uint32_t> ret;

    // We probably want to be a bit smarter about what order these are generated in to improve the
    // alpha-beta pruning later on.
    get_pawn_moves(bb, ret);
    get_king_moves(bb, ret);
    get_knight_moves(bb, ret);
    get_bishop_moves(bb, ret);
    get_rook_moves(bb, ret);
    get_queen_moves(bb, ret);

    return ret;
}