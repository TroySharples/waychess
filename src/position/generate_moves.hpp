#pragma once

// ####################################
// DECLARATION
// ####################################

#include "bitboard.hpp"

// The maximum number of legal moves in any one position (I think one with lots of queens everywhere). In reality
// it's usually about 20, but this constant is important for working out how much RAM to allocate.
constexpr std::size_t MAX_MOVES_PER_POSITION { 218 };

// Generates all legal moves, as well as moves that would be legal were it not for "checking" rules (i.e. the
// king is left in check after the move is played, the king is castling out of check, or the king is castling
// into check). We take in a span to hold our moves, and return a subspan containing the actual moves. Note
// that the input span must be large enough to hold all generated moves.
template <typename T>
std::size_t generate_pseudo_legal_moves(const bitboard& bb, std::span<T> move_buf) noexcept;

template <typename T>
std::size_t generate_pseudo_legal_loud_moves(const bitboard& bb, std::span<T> move_buf) noexcept;

// ####################################
// IMPLEMENTATION
// ####################################

#include "position/move.hpp"

namespace details
{

struct generate_move_parameters
{
    bool allow_non_captures;
    bool allow_captures;
    bool allow_promotions;
};

template <typename T>
inline std::size_t get_pawn_moves(const bitboard& bb, const generate_move_parameters& params, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_pawns { bb.boards[to_move_idx] }; to_move_pawns; )
    {
        const std::uint64_t pawn_bitboard { ls1b_isolate(to_move_pawns) };
        const std::size_t pawn_mailbox = std::countr_zero(pawn_bitboard);

        // Handle attacking moves.
        if (params.allow_captures)
        {
            for (std::uint64_t attacks { (is_black_to_play ? get_black_pawn_all_attacked_squares_from_mailbox(pawn_mailbox) : get_white_pawn_all_attacked_squares_from_mailbox(pawn_mailbox)) & (opponent_pieces | bb.en_passent_bb) }; attacks; )
            {
                const std::uint64_t attack { ls1b_isolate(attacks) };

                {
                    const std::uint32_t move { move::make_encode(pawn_mailbox, std::countr_zero(attack), to_move_idx) | move::type::CAPTURE };

                    // Test if this pawn move will result in promotion.
                    if (attack & (is_black_to_play ? RANK_1 : RANK_8)) [[unlikely]]
                    {
                        if (params.allow_promotions)
                        {
                            move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_queen,  is_black_to_play));
                            move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_knight, is_black_to_play));
                            move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_rook,   is_black_to_play));
                            move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_bishop, is_black_to_play));
                        }
                    }
                    else
                    {
                        // Remember to set the en-passent meta-bit if necessary (note this is mutually-exclusive with promotion).
                        move_buf[ret++] = move | ((attack & bb.en_passent_bb) ? move::type::EN_PASSENT : 0);
                    }
                }

                attacks ^= attack;
            }
        }

        if (params.allow_non_captures)
        {
            // Handle single pawn pushes.
            if (std::uint64_t push { is_black_to_play ? get_black_pawn_single_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) : get_white_pawn_single_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) }; push)
            {
                const std::uint32_t move { move::make_encode(pawn_mailbox, std::countr_zero(push), to_move_idx) | move::type::PAWN_PUSH_SINGLE };

                // Test if this pawn move will result in promotion.
                if (push & (is_black_to_play ? RANK_1 : RANK_8)) [[unlikely]]
                {
                    if (params.allow_promotions)
                    {
                        move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_queen,  is_black_to_play));
                        move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_knight, is_black_to_play));
                        move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_rook,   is_black_to_play));
                        move_buf[ret++] = move | move::type::PROMOTION | move::make_encode_promotion(set_piece_colour(piece_idx::w_bishop, is_black_to_play));
                    }
                }
                else
                {
                    move_buf[ret++] = move;
                }
            }

            // Handle double pawn pushes.
            if (const std::uint64_t push { is_black_to_play ? get_black_pawn_double_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) : get_white_pawn_double_push_squares_from_mailbox(pawn_mailbox, ~all_pieces) }; push)
            {
                const std::uint32_t move { move::make_encode(pawn_mailbox, std::countr_zero(push), to_move_idx) | move::type::PAWN_PUSH_DOUBLE };
                move_buf[ret++] = move;
            }
        }

        to_move_pawns ^= pawn_bitboard;
    }

    return ret;
}

template <typename T>
inline std::size_t get_king_moves(const bitboard& bb, const generate_move_parameters& params, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_king : piece_idx::w_king };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    // There's always going to be exactly one king per colour on the board - no need to ls1b-isolate.
    const std::uint64_t king_bitboard { bb.boards[to_move_idx] };
    const std::size_t king_mailbox = std::countr_zero(king_bitboard);

    // Generate pseudo-legal castling moves (we test for castling-through-check legality in make_move).
    if (params.allow_non_captures)
    {
        if (is_black_to_play)
        {
            constexpr std::size_t from_square { std::countr_zero(FILE_E & RANK_8) };

            if (bb.castling & bitboard::CASTLING_B_KS && !(all_pieces & RANK_8 & (FILE_F | FILE_G)))
            {
                constexpr std::size_t to_mb { std::countr_zero(FILE_G & RANK_8) };
                constexpr std::uint32_t move { move::make_encode(from_square, to_mb, piece_idx::b_king) | move::type::CASTLE_KS };

                move_buf[ret++] = move;
            }
            if (bb.castling & bitboard::CASTLING_B_QS && !(all_pieces & RANK_8 & (FILE_B | FILE_C | FILE_D)))
            {
                constexpr std::size_t to_mb { std::countr_zero(FILE_C & RANK_8) };
                constexpr std::uint32_t move { move::make_encode(from_square, to_mb, piece_idx::b_king) | move::type::CASTLE_QS };

                move_buf[ret++] = move;
            }
        }
        else
        {
            constexpr std::size_t from_square { std::countr_zero(FILE_E & RANK_1) };

            if (bb.castling & bitboard::CASTLING_W_KS && !(all_pieces & RANK_1 & (FILE_F | FILE_G)))
            {
                constexpr std::size_t to_mb { std::countr_zero(FILE_G & RANK_1) };
                constexpr std::uint32_t move { move::make_encode(from_square, to_mb, piece_idx::w_king) | move::type::CASTLE_KS };

                move_buf[ret++] = move;
            }
            if (bb.castling & bitboard::CASTLING_W_QS && !(all_pieces & RANK_1 & (FILE_B | FILE_C | FILE_D)))
            {
                constexpr std::size_t to_mb { std::countr_zero(FILE_C & RANK_1) };
                constexpr std::uint32_t move { move::make_encode(from_square, to_mb, piece_idx::w_king) | move::type::CASTLE_QS };

                move_buf[ret++] = move;
            }
        }
    }

    // Generate regular king moves.
    for (std::uint64_t attacks { get_king_attacked_squares_from_mailbox(king_mailbox) & ~to_move_pieces }; attacks; )
    {
        const std::uint64_t attack { ls1b_isolate(attacks) };

        const bool is_capture = attack & opponent_pieces;
        if (params.allow_captures && is_capture)
        {
            const std::uint32_t move { move::make_encode(king_mailbox, std::countr_zero(attack), to_move_idx) | move::type::CAPTURE };
            move_buf[ret++] = move;
        }
        else if (params.allow_non_captures && !is_capture)
        {
            const std::uint32_t move { move::make_encode(king_mailbox, std::countr_zero(attack), to_move_idx) };
            move_buf[ret++] = move;
        }

        attacks ^= attack;
    }

    return ret;
}

template <typename T>
inline std::size_t get_knight_moves(const bitboard& bb, const generate_move_parameters& params, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_knight : piece_idx::w_knight };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };

    for (std::uint64_t to_move_knights { bb.boards[to_move_idx] }; to_move_knights; )
    {
        const std::uint64_t knight_bitboard { ls1b_isolate(to_move_knights) };
        const std::size_t knight_mailbox = std::countr_zero(knight_bitboard);

        for (std::uint64_t attacks { get_knight_attacked_squares_from_mailbox(knight_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            const bool is_capture = attack & opponent_pieces;
            if (params.allow_captures && is_capture)
            {
                const std::uint32_t move { move::make_encode(knight_mailbox, std::countr_zero(attack), to_move_idx) | move::type::CAPTURE };
                move_buf[ret++] = move;
            }
            else if (params.allow_non_captures && !is_capture)
            {
                const std::uint32_t move { move::make_encode(knight_mailbox, std::countr_zero(attack), to_move_idx) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_knights ^= knight_bitboard;
    }

    return ret;
}

template <typename T>
inline std::size_t get_bishop_moves(const bitboard& bb, const generate_move_parameters& params, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_bishop : piece_idx::w_bishop };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_bishops { bb.boards[to_move_idx] }; to_move_bishops; )
    {
        const std::uint64_t bishop_bitboard { ls1b_isolate(to_move_bishops) };
        const std::size_t bishop_mailbox = std::countr_zero(bishop_bitboard);

        for (std::uint64_t attacks { get_bishop_attacked_squares_from_mailbox(all_pieces, bishop_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            const bool is_capture = attack & opponent_pieces;
            if (params.allow_captures && is_capture)
            {
                const std::uint32_t move { move::make_encode(bishop_mailbox, std::countr_zero(attack), to_move_idx) | move::type::CAPTURE };
                move_buf[ret++] = move;
            }
            else if (params.allow_non_captures && !is_capture)
            {
                const std::uint32_t move { move::make_encode(bishop_mailbox, std::countr_zero(attack), to_move_idx) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_bishops ^= bishop_bitboard;
    }

    return ret;
}

template <typename T>
inline std::size_t get_rook_moves(const bitboard& bb, const generate_move_parameters& params, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_rook : piece_idx::w_rook };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_rooks { bb.boards[to_move_idx] }; to_move_rooks; )
    {
        const std::uint64_t rook_bitboard { ls1b_isolate(to_move_rooks) };
        const std::size_t rook_mailbox = std::countr_zero(rook_bitboard);

        for (std::uint64_t attacks { get_rook_attacked_squares_from_mailbox(all_pieces, rook_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            const bool is_capture = attack & opponent_pieces;
            if (params.allow_captures && is_capture)
            {
                const std::uint32_t move { move::make_encode(rook_mailbox, std::countr_zero(attack), to_move_idx) | move::type::CAPTURE };
                move_buf[ret++] = move;
            }
            else if (params.allow_non_captures && !is_capture)
            {
                const std::uint32_t move { move::make_encode(rook_mailbox, std::countr_zero(attack), to_move_idx) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_rooks ^= rook_bitboard;
    }

    return ret;
}

template <typename T>
inline std::size_t get_queen_moves(const bitboard& bb, const generate_move_parameters& params, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    const bool is_black_to_play { bb.is_black_to_play() };
    const piece_idx to_move_idx { is_black_to_play ? piece_idx::b_queen : piece_idx::w_queen };
    const std::uint64_t to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };
    const std::uint64_t opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };
    const std::uint64_t all_pieces { to_move_pieces | opponent_pieces };

    for (std::uint64_t to_move_queens { bb.boards[to_move_idx] }; to_move_queens; )
    {
        const std::uint64_t queen_bitboard { ls1b_isolate(to_move_queens) };
        const std::size_t queen_mailbox = std::countr_zero(queen_bitboard);

        for (std::uint64_t attacks { get_queen_attacked_squares_from_mailbox(all_pieces, queen_mailbox) & ~to_move_pieces }; attacks; )
        {
            const std::uint64_t attack { ls1b_isolate(attacks) };

            const bool is_capture = attack & opponent_pieces;
            if (params.allow_captures && is_capture)
            {
                const std::uint32_t move { move::make_encode(queen_mailbox, std::countr_zero(attack), to_move_idx) | move::type::CAPTURE };
                move_buf[ret++] = move;
            }
            else if (params.allow_non_captures && !is_capture)
            {
                const std::uint32_t move { move::make_encode(queen_mailbox, std::countr_zero(attack), to_move_idx) };
                move_buf[ret++] = move;
            }

            attacks ^= attack;
        }

        to_move_queens ^= queen_bitboard;
    }

    return ret;
}

}

template <typename T>
inline std::size_t generate_pseudo_legal_moves(const bitboard& bb, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    // Allow all moves.
    const details::generate_move_parameters params { .allow_non_captures=true, .allow_captures=true, .allow_promotions=true };

    ret += details::get_pawn_moves(bb,   params, move_buf.subspan(ret));
    ret += details::get_king_moves(bb,   params, move_buf.subspan(ret));
    ret += details::get_knight_moves(bb, params, move_buf.subspan(ret));
    ret += details::get_bishop_moves(bb, params, move_buf.subspan(ret));
    ret += details::get_rook_moves(bb,   params, move_buf.subspan(ret));
    ret += details::get_queen_moves(bb,  params, move_buf.subspan(ret));

    return ret;
}

template <typename T>
inline std::size_t generate_pseudo_legal_loud_moves(const bitboard& bb, std::span<T> move_buf) noexcept
{
    std::size_t ret {};

    // Only allow captures - we might want to change this to also include promotions at some point.
    const details::generate_move_parameters params { .allow_non_captures=false, .allow_captures=true, .allow_promotions=false };

    ret += details::get_pawn_moves(bb,   params, move_buf.subspan(ret));
    ret += details::get_king_moves(bb,   params, move_buf.subspan(ret));
    ret += details::get_knight_moves(bb, params, move_buf.subspan(ret));
    ret += details::get_bishop_moves(bb, params, move_buf.subspan(ret));
    ret += details::get_rook_moves(bb,   params, move_buf.subspan(ret));
    ret += details::get_queen_moves(bb,  params, move_buf.subspan(ret));

    return ret;
}