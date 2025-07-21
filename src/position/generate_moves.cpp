#include "generate_moves.hpp"

#include "move.hpp"

namespace
{

std::size_t get_pawn_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_pawns { bb.boards[to_move_idx] }; to_move_pawns; )
    {
        const std::uint64_t pawn_bitboard { ls1b_isolate(to_move_pawns) };
        const std::uint8_t pawn_mailbox = std::countr_zero(pawn_bitboard);

        // Handle attacking moves.
        for (std::uint64_t attacks { (is_black_to_play ? get_black_pawn_all_attacked_squares_from_mailbox(pawn_mailbox) : get_white_pawn_all_attacked_squares_from_mailbox(pawn_mailbox)) & (opponent_pieces | bb.en_passent_bb) }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            {
                const std::uint32_t move { move::serialise(pawn_mailbox, std::countr_zero(attack), to_move_idx, move::move_type::CAPTURE, 0, bb.castling, bb.en_passent_bb) };

                // Test if this pawn move will result in promotion.
                if (attack & (is_black_to_play ? RANK_1 : RANK_8))
                {
                    move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_QUEEN);
                    move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_KNIGHT);
                    move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_ROOK);
                    move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_BISHOP);
                }
                else
                {
                    // Remember to set the en-passent meta-bit if necessary (note this is mutually-exclusive with promotion).
                    move_buf[ret++] = move | (attack & bb.en_passent_bb ? move::serialise_move_type(move::move_type::EN_PASSENT) : 0);
                }
            }

            attacks ^= attack;
        }

        // Handle single pawn pushes.
        if (std::uint64_t push { is_black_to_play ? get_black_pawn_single_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) : get_white_pawn_single_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) }; push)
        {
            const std::uint32_t move { move::serialise(pawn_mailbox, std::countr_zero(push), to_move_idx, move::move_type::PAWN_PUSH, move::move_info::PAWN_PUSH_SINGLE, bb.castling, bb.en_passent_bb)};

            // Test if this pawn move will result in promotion.
            if (push & (is_black_to_play ? RANK_1 : RANK_8))
            {
                move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_QUEEN);
                move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_KNIGHT);
                move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_ROOK);
                move_buf[ret++] = move | move::serialise_move_type(move::move_type::PROMOTION) | move::serialise_move_info(move::move_info::PROMOTION_BISHOP);
            }
            else
            {
                move_buf[ret++] = move;
            }
        }

        // Handle double pawn pushes.
        if (const std::uint64_t push { is_black_to_play ? get_black_pawn_double_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) : get_white_pawn_double_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) }; push)
        {
            const std::uint32_t move { move::serialise(pawn_mailbox, std::countr_zero(push), to_move_idx, move::move_type::PAWN_PUSH, move::move_info::PAWN_PUSH_DOUBLE, bb.castling, bb.en_passent_bb) };
            move_buf[ret++] = move;
        }

        to_move_pawns ^= pawn_bitboard;
    }

    return ret;
}

std::size_t get_king_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_king : piece_idx::w_king };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    // There's always going to be exactly one king per colour on the board - no need to ls1b-isolate.
    const std::uint64_t king_bitboard { bb.boards[to_move_idx] };
    const std::uint8_t king_mailbox = std::countr_zero(king_bitboard);

    // Generate regular king moves.
    for (std::uint64_t attacks { get_king_attacked_squares_from_mailbox(king_mailbox) & ~to_move_pieces }; attacks; )
    {
        const std::uint64_t attack { ls1b_isolate(attacks) };

        {
            const std::uint32_t move { move::serialise(king_mailbox, std::countr_zero(attack), to_move_idx, attack & opponent_pieces ? move::move_type::CAPTURE : 0, 0, bb.castling, bb.en_passent_bb) };
            move_buf[ret++] = move;
        }

        attacks ^= attack;
    }

    // Generate pseudo-legal castling moves (we test for castling-through-check legality in make_move).
    if (is_black_to_play)
    {
        constexpr std::uint8_t from_square { std::countr_zero(FILE_E & RANK_8) };

        if (bb.castling & bitboard::CASTLING_B_KS && !(all_pieces & RANK_8 & (FILE_F | FILE_G)))
        {
            constexpr std::uint8_t to_mb { std::countr_zero(FILE_G & RANK_8) };
            const std::uint32_t move { move::serialise(from_square, to_mb, piece_idx::b_king, move::move_type::CASTLE, move::move_info::CASTLE_KS, bb.castling, bb.en_passent_bb) };

            move_buf[ret++] = move;
        }
        if (bb.castling & bitboard::CASTLING_B_QS && !(all_pieces & RANK_8 & (FILE_B | FILE_C | FILE_D)))
        {
            constexpr std::uint8_t to_mb { std::countr_zero(FILE_C & RANK_8) };
            const std::uint32_t move { move::serialise(from_square, to_mb, piece_idx::b_king, move::move_type::CASTLE, move::move_info::CASTLE_QS, bb.castling, bb.en_passent_bb) };

            move_buf[ret++] = move;
        }
    }
    else
    {
        constexpr std::uint8_t from_square { std::countr_zero(FILE_E & RANK_1) };

        if (bb.castling & bitboard::CASTLING_W_KS && !(all_pieces & RANK_1 & (FILE_F | FILE_G)))
        {
            constexpr std::uint8_t to_mb { std::countr_zero(FILE_G & RANK_1) };
            const std::uint32_t move { move::serialise(from_square, to_mb, piece_idx::w_king, move::move_type::CASTLE, move::move_info::CASTLE_KS, bb.castling, bb.en_passent_bb) };

            move_buf[ret++] = move;
        }
        if (bb.castling & bitboard::CASTLING_W_QS && !(all_pieces & RANK_1 & (FILE_B | FILE_C | FILE_D)))
        {
            constexpr std::uint8_t to_mb { std::countr_zero(FILE_C & RANK_1) };
            const std::uint32_t move { move::serialise(from_square, to_mb, piece_idx::w_king, move::move_type::CASTLE, move::move_info::CASTLE_QS, bb.castling, bb.en_passent_bb) };

            move_buf[ret++] = move;
        }
    }

    return ret;
}

std::size_t get_knight_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_knight : piece_idx::w_knight };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };

    for (std::uint64_t to_move_knights { bb.boards[to_move_idx] }; to_move_knights; )
    {
        const std::uint64_t knight_bitboard { ls1b_isolate(to_move_knights) };
        const std::uint8_t knight_mailbox = std::countr_zero(knight_bitboard);

        for (std::uint64_t attacks { get_knight_attacked_squares_from_mailbox(knight_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            {
                const std::uint32_t move { move::serialise(knight_mailbox, std::countr_zero(attack), to_move_idx, attack & opponent_pieces ? move::move_type::CAPTURE : 0, 0, bb.castling, bb.en_passent_bb) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_knights ^= knight_bitboard;
    }

    return ret;
}

std::size_t get_bishop_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_bishop : piece_idx::w_bishop };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_bishops { bb.boards[to_move_idx] }; to_move_bishops; )
    {
        const std::uint64_t bishop_bitboard { ls1b_isolate(to_move_bishops) };
        const std::uint8_t bishop_mailbox = std::countr_zero(bishop_bitboard);

        for (std::uint64_t attacks { get_bishop_attacked_squares_from_mailbox(bishop_mailbox, all_pieces) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            {
                const std::uint32_t move { move::serialise(bishop_mailbox, std::countr_zero(attack), to_move_idx, attack & opponent_pieces ? move::move_type::CAPTURE : 0, 0, bb.castling, bb.en_passent_bb) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_bishops ^= bishop_bitboard;
    }

    return ret;
}

std::size_t get_rook_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_rook : piece_idx::w_rook };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_rooks { bb.boards[to_move_idx] }; to_move_rooks; )
    {
        const std::uint64_t rook_bitboard { ls1b_isolate(to_move_rooks) };
        const std::uint8_t rook_mailbox = std::countr_zero(rook_bitboard);

        for (std::uint64_t attacks { get_rook_attacked_squares_from_mailbox(rook_mailbox, all_pieces) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            {
                const std::uint32_t move { move::serialise(rook_mailbox, std::countr_zero(attack), to_move_idx, attack & opponent_pieces ? move::move_type::CAPTURE : 0, 0, bb.castling, bb.en_passent_bb) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_rooks ^= rook_bitboard;
    }

    return ret;
}

std::size_t get_queen_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_queen : piece_idx::w_queen };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.b_pieces : bb.w_pieces };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_queens { bb.boards[to_move_idx] }; to_move_queens; )
    {
        const std::uint64_t queen_bitboard { ls1b_isolate(to_move_queens) };
        const std::uint8_t queen_mailbox = std::countr_zero(queen_bitboard);

        for (std::uint64_t attacks { get_queen_attacked_squares_from_mailbox(queen_mailbox, all_pieces) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            {
                const std::uint32_t move { move::serialise(queen_mailbox, std::countr_zero(attack), to_move_idx, attack & opponent_pieces ? move::move_type::CAPTURE : 0, 0, bb.castling, bb.en_passent_bb) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_queens ^= queen_bitboard;
    }

    return ret;
}

}

std::size_t generate_pseudo_legal_moves(const bitboard& bb, std::span<std::uint32_t> move_buf)
{
    std::size_t ret {};

    // We probably want to be a bit smarter about what order these are generated in to improve the alpha-beta pruning
    // later on. This could look like just generating piece moves that are likely to be good first, or possibly running
    // some sort of sorting algorithm on the moves vector before returning.

    ret += get_pawn_moves(bb, move_buf.subspan(ret));
    ret += get_king_moves(bb, move_buf.subspan(ret));
    ret += get_knight_moves(bb, move_buf.subspan(ret));
    ret += get_bishop_moves(bb, move_buf.subspan(ret));
    ret += get_rook_moves(bb, move_buf.subspan(ret));
    ret += get_queen_moves(bb, move_buf.subspan(ret));

    return ret;
}