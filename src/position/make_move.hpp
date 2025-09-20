#pragma once

// ####################################
// DECLARATION
// ####################################

#include "evaluation/evaluate.hpp"
#include "game_state.hpp"
#include "pieces/pieces.hpp"
#include <bit>

struct make_move_args
{
    bool check_legality;
};

bool make_move(const make_move_args& args, bitboard& bb,   std::uint32_t make) noexcept;
bool make_move(const make_move_args& args, bitboard& bb,   std::uint32_t make, std::uint32_t& unmake) noexcept;
bool make_move(const make_move_args& args, bitboard& bb,   std::uint32_t make, std::uint32_t& unmake, std::uint64_t& hash) noexcept;
bool make_move(const make_move_args& args, bitboard& bb,   std::uint32_t make, std::uint32_t& unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept;
bool make_move(const make_move_args& args, game_state& gs, std::uint32_t make) noexcept;
bool make_move(const make_move_args& args, game_state& gs, std::uint32_t make, std::uint32_t& unmake) noexcept;

void unmake_move(bitboard& bb,   std::uint32_t make, std::uint32_t unmake) noexcept;
void unmake_move(bitboard& bb,   std::uint32_t make, std::uint32_t unmake, std::uint64_t& hash) noexcept;
void unmake_move(bitboard& bb,   std::uint32_t make, std::uint32_t unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept;
void unmake_move(game_state& gs, std::uint32_t make, std::uint32_t unmake) noexcept;
void unmake_move(bitboard& bb,   std::uint64_t make_unmake) noexcept;
void unmake_move(bitboard& bb,   std::uint64_t make_unmake, std::uint64_t& hash) noexcept;
void unmake_move(bitboard& bb,   std::uint64_t make_unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept;
void unmake_move(game_state& gs, std::uint64_t make_unmake) noexcept;

// ####################################
// IMPLEMENTATION
// ####################################

#include "position/move.hpp"
#include "position/attacks.hpp"
#include "position/zobrist_hash.hpp"

namespace details
{

inline bool make_move_impl(const make_move_args& args, bitboard& bb, std::uint32_t make, std::uint32_t& unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept
{
    bool ret { true };

    unmake = 0;

    const bool is_black_to_play   { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };

    // The move only contains the piece up to colour - exactly which colour piece must be derived from the side-to-play.
    const std::size_t from_mb { move::make_decode_from_mb(make) };
    const std::size_t to_mb   { move::make_decode_to_mb(make) };
    const piece_idx piece     { move::make_decode_piece_idx(make) };

    // All of the above information is overwritten if this is a null-move (the value 0).
    const bool is_null { make == move::NULL_MOVE };

    const std::uint64_t from_bb    { 1ULL << from_mb };
    const std::uint64_t to_bb      { 1ULL << to_mb };
    const std::uint64_t from_to_bb { from_bb | to_bb };

    // Either do or undo black as the side to move.
    hash ^= zobrist::CODE_IS_BLACK_TO_MOVE;

    // Fill out the unmake info.
    unmake |= move::unmake_encode_ply_50m(bb.ply_50m);
    unmake |= move::unmake_encode_castling(bb.castling);
    if (bb.en_passent_bb) [[unlikely]]
    {
        unmake |= move::unmake_encode_en_passent(bb.en_passent_bb);

        hash ^= zobrist::get_code_en_passent(std::countr_zero(bb.en_passent_bb));
        bb.en_passent_bb = 0;
    }

    // Handle the removal of castling rights. We use the from-to bitboard instead of just the from one as we also have
    // to remember to remove castling rights in the event one of our rooks gets captured - this is actually quite subtle...
    if (from_to_bb & ((FILE_E & RANK_1) | (FILE_A & RANK_1))) [[unlikely]]
    {
        hash        ^= zobrist::get_code_castling(bb.castling);
        bb.castling &= ~bitboard::CASTLING_W_QS;
        hash        ^= zobrist::get_code_castling(bb.castling);
    }
    if (from_to_bb & ((FILE_E & RANK_1) | (FILE_H & RANK_1))) [[unlikely]]
    {
        hash        ^= zobrist::get_code_castling(bb.castling);
        bb.castling &= ~bitboard::CASTLING_W_KS;
        hash        ^= zobrist::get_code_castling(bb.castling);
    }
    if (from_to_bb & ((FILE_E & RANK_8) | (FILE_A & RANK_8))) [[unlikely]]
    {
        hash        ^= zobrist::get_code_castling(bb.castling);
        bb.castling &= ~bitboard::CASTLING_B_QS;
        hash        ^= zobrist::get_code_castling(bb.castling);
    }
    if (from_to_bb & ((FILE_E & RANK_8) | (FILE_H & RANK_8))) [[unlikely]]
    {
        hash        ^= zobrist::get_code_castling(bb.castling);
        bb.castling &= ~bitboard::CASTLING_B_KS;
        hash        ^= zobrist::get_code_castling(bb.castling);
    }

    // Next we handle mechanically moving the side-to-plays piece. We have to be careful about the special case of
    // pawn promotions when updating the piece-specific bitboard.
    if (!is_null) [[likely]]
    {
        to_move_pieces ^= from_to_bb;
        hash           ^= zobrist::get_code_piece(piece, from_mb);

        eval.mg -= (evaluation::piece_mg_evaluation[piece] + evaluation::piece_square_mg_evaluation[piece][from_mb]);
        eval.eg -= (evaluation::piece_eg_evaluation[piece] + evaluation::piece_square_eg_evaluation[piece][from_mb]);

        if (make & move::type::PROMOTION) [[unlikely]]
        {
            const piece_idx promotion_idx { move::make_decode_promotion(make) };

            bb.boards[piece]         ^= from_bb;
            bb.boards[promotion_idx] ^= to_bb;
            hash                     ^= zobrist::get_code_piece(promotion_idx, to_mb);

            // Note that pawns don't affect the game-phase.
            eval.mg += evaluation::piece_mg_evaluation[promotion_idx] + evaluation::piece_square_mg_evaluation[promotion_idx][to_mb];
            eval.eg += evaluation::piece_eg_evaluation[promotion_idx] + evaluation::piece_square_eg_evaluation[promotion_idx][to_mb];
            eval.gp += evaluation::piece_gp[promotion_idx];
        }
        else
        {
            bb.boards[piece] ^= from_to_bb;
            hash             ^= zobrist::get_code_piece(piece, to_mb);

            eval.mg += evaluation::piece_mg_evaluation[piece] + evaluation::piece_square_mg_evaluation[piece][to_mb];
            eval.eg += evaluation::piece_eg_evaluation[piece] + evaluation::piece_square_eg_evaluation[piece][to_mb];
        }
    }

    // Handle piece captures (this should be compatible with null-moves).
    if (make & move::type::CAPTURE)
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };

        // En-passent captures are special - the piece to be taken off the board isn't the target move square.
        if (make & move::type::EN_PASSENT) [[unlikely]]
        {
            const std::uint64_t capture_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);
            const std::uint64_t capture_mb = std::countr_zero(capture_bb);
            const piece_idx capture_idx { static_cast<piece_idx>(is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn) };

            opponent_pieces        ^= capture_bb;
            bb.boards[capture_idx] ^= capture_bb;
            hash                   ^= zobrist::get_code_piece(capture_idx, capture_mb);

            // Note that pawns don't affect the game-phase.
            eval.mg -= (evaluation::piece_mg_evaluation[capture_idx] + evaluation::piece_square_mg_evaluation[capture_idx][capture_mb]);
            eval.eg -= (evaluation::piece_eg_evaluation[capture_idx] + evaluation::piece_square_eg_evaluation[capture_idx][capture_mb]);
        }
        // For regular piece captures, we have to loop through the opponents piece bitboards to find the one containing
        // the piece to remove. We also have to remember to fill in the unmake-move so we can recover the piece when
        // going in reverse.
        else
        {
            const auto capture_idx = bb.get_piece_type_colour(to_bb, !is_black_to_play);
            unmake |= move::unmake_encode_capture(static_cast<piece_idx>(capture_idx));

            opponent_pieces        ^= to_bb;
            bb.boards[capture_idx] ^= to_bb;
            hash                   ^= zobrist::get_code_piece(static_cast<piece_idx>(capture_idx), to_mb);

            eval.mg -= (evaluation::piece_mg_evaluation[capture_idx] + evaluation::piece_square_mg_evaluation[capture_idx][to_mb]);
            eval.eg -= (evaluation::piece_eg_evaluation[capture_idx] + evaluation::piece_square_eg_evaluation[capture_idx][to_mb]);
            eval.gp -= evaluation::piece_gp[capture_idx];
        }

        bb.ply_50m = 0;
    }
    else if (make & move::type::PAWN_PUSH_SINGLE)
    {
        // If the move was just a single pawn push we need to reset the ply counter for the 50 move rule.
        bb.ply_50m = 0;
    }
    else if (make & move::type::PAWN_PUSH_DOUBLE)
    {
        // If the move was a double pawn push we also have to update the en-passent target square for the next move.
        bb.ply_50m = 0;

        bb.en_passent_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);
        hash ^= zobrist::get_code_en_passent(std::countr_zero(bb.en_passent_bb));
    }
    else
    {
        // Otherwise we can increment the counter towards the 50 move limit.
        bb.ply_50m++;
    }

    // Handle moving the rook for castling (legality is checked below).
    if (make & move::type::CASTLE_KS) [[unlikely]]
    {
        if (is_black_to_play)
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_H & RANK_8) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_F & RANK_8) };
            constexpr std::uint64_t from_to_bb { (FILE_H & RANK_8) | (FILE_F & RANK_8) };

            bb.boards[piece_idx::b_rook] ^= from_to_bb;
            bb.boards[piece_idx::b_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::b_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::b_rook, to_mb)
            };
            hash ^= code;

            eval.mg += (evaluation::piece_square_mg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::b_rook][from_mb]);
            eval.eg += (evaluation::piece_square_eg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::b_rook][from_mb]);
        }
        else
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_H & RANK_1) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_F & RANK_1) };
            constexpr std::uint64_t from_to_bb { (FILE_H & RANK_1) | (FILE_F & RANK_1) };

            bb.boards[piece_idx::w_rook] ^= from_to_bb;
            bb.boards[piece_idx::w_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::w_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::w_rook, to_mb)
            };
            hash ^= code;

            eval.mg += (evaluation::piece_square_mg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::w_rook][from_mb]);
            eval.eg += (evaluation::piece_square_eg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::w_rook][from_mb]);
        }
    }
    else if (make & move::type::CASTLE_QS) [[unlikely]]
    {
        if (is_black_to_play)
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_A & RANK_8) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_D & RANK_8) };
            constexpr std::uint64_t from_to_bb { (FILE_A & RANK_8) | (FILE_D & RANK_8) };

            bb.boards[piece_idx::b_rook] ^= from_to_bb;
            bb.boards[piece_idx::b_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::b_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::b_rook, to_mb)
            };
            hash ^= code;

            eval.mg += (evaluation::piece_square_mg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::b_rook][from_mb]);
            eval.eg += (evaluation::piece_square_eg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::b_rook][from_mb]);
        }
        else
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_A & RANK_1) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_D & RANK_1) };
            constexpr std::uint64_t from_to_bb { (FILE_A & RANK_1) | (FILE_D & RANK_1) };

            bb.boards[piece_idx::w_rook] ^= from_to_bb;
            bb.boards[piece_idx::w_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::w_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::w_rook, to_mb)
            };
            hash ^= code;

            eval.mg += (evaluation::piece_square_mg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::w_rook][from_mb]);
            eval.eg += (evaluation::piece_square_eg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::w_rook][from_mb]);
        }
    }

    // Handle left-in-check legality checking.
    if (args.check_legality)
    {
        if (make & move::type::CASTLE_KS) [[unlikely]]
        {
            if (is_black_to_play)
                ret = !get_attackers_white(bb, std::countr_zero(FILE_E & RANK_8), std::countr_zero(FILE_F & RANK_8), std::countr_zero(FILE_G & RANK_8));
            else
                ret = !get_attackers_black(bb, std::countr_zero(FILE_E & RANK_1), std::countr_zero(FILE_F & RANK_1), std::countr_zero(FILE_G & RANK_1));
        }
        else if (make & move::type::CASTLE_QS) [[unlikely]]
        {
            if (is_black_to_play)
                ret = !get_attackers_white(bb, std::countr_zero(FILE_E & RANK_8), std::countr_zero(FILE_D & RANK_8), std::countr_zero(FILE_C & RANK_8));
            else
                ret = !get_attackers_black(bb, std::countr_zero(FILE_E & RANK_1), std::countr_zero(FILE_D & RANK_1), std::countr_zero(FILE_C & RANK_1));
        }
        else
        {
            ret = !is_in_check(bb, is_black_to_play);
        }
    }

    return ret;
}

inline void unmake_move_impl(bitboard& bb, std::uint32_t make, std::uint32_t unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept
{
    const bool is_black_to_play   { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };

    // The move only contains the piece up to colour - exactly which colour piece must be derived from the side-to-play.
    const std::size_t from_mb   { move::make_decode_from_mb(make) };
    const std::size_t to_mb     { move::make_decode_to_mb(make) };
    const piece_idx piece       { move::make_decode_piece_idx(make) };

    // All of the above information is overwritten if this is a null-move (the LSBs are 0 - the MSBs will contain unmake information like normal).
    const bool is_null { make == move::NULL_MOVE };

    const std::uint64_t from_bb    { 1ULL << from_mb };
    const std::uint64_t to_bb      { 1ULL << to_mb };
    const std::uint64_t from_to_bb { from_bb | to_bb };

    // Either do or undo black as the side to move.
    hash ^= zobrist::CODE_IS_BLACK_TO_MOVE;

    // Undo any current en-passent squares from the Zobrist hash.
    if (bb.en_passent_bb)
        hash ^= zobrist::get_code_en_passent(std::countr_zero(bb.en_passent_bb));

    // Handle en-passent / 50m-ply re-establishment.
    bb.ply_50m       = move::unmake_decode_ply_50m(unmake);
    bb.en_passent_bb = move::unmake_decode_en_passent(unmake);
    if (bb.en_passent_bb) [[unlikely]]
        hash ^= zobrist::get_code_en_passent(std::countr_zero(bb.en_passent_bb));

    // Set castling rights.
    if (const std::uint8_t castling { move::unmake_decode_castling(unmake) }; bb.castling != castling) [[unlikely]]
    {
        hash ^= zobrist::get_code_castling(bb.castling) ^ zobrist::get_code_castling(castling);

        bb.castling = castling;
    }

    // Handle mechanically moving the pieces (from the to-square to the from-square).
    if (!is_null) [[likely]]
    {
        to_move_pieces ^= from_to_bb;
        hash           ^= zobrist::get_code_piece(piece, from_mb);

        eval.mg += evaluation::piece_mg_evaluation[piece] + evaluation::piece_square_mg_evaluation[piece][from_mb];
        eval.eg += evaluation::piece_eg_evaluation[piece] + evaluation::piece_square_eg_evaluation[piece][from_mb];

        if (make & move::type::PROMOTION) [[unlikely]]
        {
            bb.boards[piece] ^= from_bb;

            const auto promotion_idx = move::make_decode_promotion(make);
            hash                     ^= zobrist::get_code_piece(promotion_idx, to_mb);
            bb.boards[promotion_idx] ^= to_bb;

            // Note that pawns don't affect the game-phase.
            eval.mg -= (evaluation::piece_mg_evaluation[promotion_idx] + evaluation::piece_square_mg_evaluation[promotion_idx][to_mb]);
            eval.eg -= (evaluation::piece_eg_evaluation[promotion_idx] + evaluation::piece_square_eg_evaluation[promotion_idx][to_mb]);
            eval.gp -= evaluation::piece_gp[promotion_idx];
        }
        else
        {
            hash ^= zobrist::get_code_piece(piece, to_mb);
            bb.boards[piece] ^= from_to_bb;

            eval.mg -= (evaluation::piece_mg_evaluation[piece] + evaluation::piece_square_mg_evaluation[piece][to_mb]);
            eval.eg -= (evaluation::piece_eg_evaluation[piece] + evaluation::piece_square_eg_evaluation[piece][to_mb]);
        }
    }

    // Handle piece captures.
    if (make & move::type::CAPTURE)
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };

        // En-passent captures are special - the piece to be taken off the board isn't the target move square.
        if (make & move::type::EN_PASSENT) [[unlikely]]
        {
            const std::uint64_t capture_bb = is_black_to_play ? (to_bb >> 8) : (to_bb << 8);
            const std::uint64_t capture_mb = std::countr_zero(capture_bb);
            const piece_idx capture_idx { static_cast<piece_idx>(is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn) };

            opponent_pieces        ^= capture_bb;
            bb.boards[capture_idx] ^= capture_bb;
            hash                   ^= zobrist::get_code_piece(capture_idx, capture_mb);

            // Note that pawns don't affect the game-phase.
            eval.mg += evaluation::piece_mg_evaluation[capture_idx] + evaluation::piece_square_mg_evaluation[capture_idx][capture_mb];;
            eval.eg += evaluation::piece_eg_evaluation[capture_idx] + evaluation::piece_square_eg_evaluation[capture_idx][capture_mb];;
        }
        else
        {
            const std::uint64_t capture_mb = std::countr_zero(to_bb);
            const auto capture_idx = move::unmake_decode_capture(unmake);

            opponent_pieces        ^= to_bb;
            bb.boards[capture_idx] ^= to_bb;
            hash                   ^= zobrist::get_code_piece(capture_idx, capture_mb);

            eval.mg += evaluation::piece_mg_evaluation[capture_idx] + evaluation::piece_square_mg_evaluation[capture_idx][to_mb];
            eval.eg += evaluation::piece_eg_evaluation[capture_idx] + evaluation::piece_square_eg_evaluation[capture_idx][to_mb];
            eval.gp += evaluation::piece_gp[capture_idx];
        }
    }

    // Handle moving the rook for castling.
    if (make & move::type::CASTLE_KS) [[unlikely]]
    {
        if (!is_black_to_play)
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_H & RANK_8) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_F & RANK_8) };
            constexpr std::uint64_t from_to_bb { (FILE_H & RANK_8) | (FILE_F & RANK_8) };

            bb.boards[piece_idx::b_rook] ^= from_to_bb;
            bb.boards[piece_idx::b_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::b_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::b_rook, to_mb)
            };
            hash ^= code;

            eval.mg -= (evaluation::piece_square_mg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::b_rook][from_mb]);
            eval.eg -= (evaluation::piece_square_eg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::b_rook][from_mb]);
        }
        else
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_H & RANK_1) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_F & RANK_1) };
            constexpr std::uint64_t from_to_bb { (FILE_H & RANK_1) | (FILE_F & RANK_1) };

            bb.boards[piece_idx::w_rook] ^= from_to_bb;
            bb.boards[piece_idx::w_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::w_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::w_rook, to_mb)
            };
            hash ^= code;

            eval.mg -= (evaluation::piece_square_mg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::w_rook][from_mb]);
            eval.eg -= (evaluation::piece_square_eg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::w_rook][from_mb]);
        }
    }
    else if (make & move::type::CASTLE_QS) [[unlikely]]
    {
        if (!is_black_to_play)
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_A & RANK_8) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_D & RANK_8) };
            constexpr std::uint64_t from_to_bb { (FILE_A & RANK_8) | (FILE_D & RANK_8) };

            bb.boards[piece_idx::b_rook] ^= from_to_bb;
            bb.boards[piece_idx::b_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::b_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::b_rook, to_mb)
            };
            hash ^= code;

            eval.mg -= (evaluation::piece_square_mg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::b_rook][from_mb]);
            eval.eg -= (evaluation::piece_square_eg_evaluation[piece_idx::b_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::b_rook][from_mb]);
        }
        else
        {
            constexpr std::size_t from_mb      { std::countr_zero(FILE_A & RANK_1) };
            constexpr std::size_t to_mb        { std::countr_zero(FILE_D & RANK_1) };
            constexpr std::uint64_t from_to_bb { (FILE_A & RANK_1) | (FILE_D & RANK_1) };

            bb.boards[piece_idx::w_rook] ^= from_to_bb;
            bb.boards[piece_idx::w_any]  ^= from_to_bb;

            constexpr std::uint64_t code {
                zobrist::get_code_piece(piece_idx::w_rook, from_mb)
              ^ zobrist::get_code_piece(piece_idx::w_rook, to_mb)
            };
            hash ^= code;

            eval.mg -= (evaluation::piece_square_mg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_mg_evaluation[piece_idx::w_rook][from_mb]);
            eval.eg -= (evaluation::piece_square_eg_evaluation[piece_idx::w_rook][to_mb] - evaluation::piece_square_eg_evaluation[piece_idx::w_rook][from_mb]);
        }
    }
}

}

inline bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t make) noexcept
{
    // Dummy unmake (will get hopefully get optimised away).
    std::uint32_t unmake_dummy;
    return make_move(args, bb, make, unmake_dummy);
}

inline bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t make, std::uint32_t& unmake) noexcept
{
    // Dummy hash (will hopefully get optimised away).
    std::uint64_t hash_dummy {};
    return make_move(args, bb, make, unmake, hash_dummy);
}

inline bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t make, std::uint32_t& unmake, std::uint64_t& hash) noexcept
{
    // Dummy (will hopefully get optimised away).
    evaluation::eval eval_dummy;
    return make_move(args, bb, make, unmake, hash, eval_dummy);
}

inline bool make_move(const make_move_args& args, bitboard& bb,   std::uint32_t make, std::uint32_t& unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept
{
    const bool ret { details::make_move_impl(args, bb, make, unmake, hash, eval) };

    // Increment the ply-counter.
    bb.ply_counter++;

    return ret;
}

inline bool make_move(const make_move_args& args, game_state& gs, std::uint32_t make) noexcept
{
    // Dummy unmake (will get hopefully get optimised away).
    std::uint32_t unmake_dummy;
    return make_move(args, gs, make, unmake_dummy);
}

inline bool make_move(const make_move_args& args, game_state& gs, std::uint32_t make, std::uint32_t& unmake) noexcept
{
    const bool ret { details::make_move_impl(args, gs.bb, make, unmake, gs.hash, gs.eval) };

    // Add our move to our game-state history and increment the ply-counter.
    gs.position_history[++gs.bb.ply_counter] = gs.hash;

    return ret;
}

inline void unmake_move(bitboard& bb, std::uint32_t make, std::uint32_t unmake) noexcept
{
    // Dummy hash (will hopefully get optimised away).
    std::uint64_t hash_dummy {};
    unmake_move(bb, make, unmake, hash_dummy);
}

inline void unmake_move(bitboard& bb, std::uint32_t make, std::uint32_t unmake, std::uint64_t& hash) noexcept
{
    // Dummy (will hopefully get optimised away).
    evaluation::eval eval_dummy;
    unmake_move(bb, make, unmake, hash, eval_dummy);
}

inline void unmake_move(bitboard& bb, std::uint32_t make, std::uint32_t unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept
{
    details::unmake_move_impl(bb, make, unmake, hash, eval);

    // Decrement the ply-counter.
    bb.ply_counter--;
}

inline void unmake_move(game_state& gs, std::uint32_t make, std::uint32_t unmake) noexcept
{
    details::unmake_move_impl(gs.bb, make, unmake, gs.hash, gs.eval);

    // Decrement the ply-counter.
    gs.bb.ply_counter--;
}

inline void unmake_move(bitboard& bb, std::uint64_t make_unmake) noexcept
{
    const std::uint32_t make   = make_unmake;
    const std::uint32_t unmake = make_unmake >> 32;

    unmake_move(bb, make, unmake);
}

inline void unmake_move(bitboard& bb, std::uint64_t make_unmake, std::uint64_t& hash) noexcept
{
    const std::uint32_t make   = make_unmake;
    const std::uint32_t unmake = make_unmake >> 32;

    unmake_move(bb, make, unmake, hash);
}

inline void unmake_move(bitboard& bb, std::uint64_t make_unmake, std::uint64_t& hash, evaluation::eval& eval) noexcept
{
    const std::uint32_t make   = make_unmake;
    const std::uint32_t unmake = make_unmake >> 32;

    unmake_move(bb, make, unmake, hash, eval);
}

inline void unmake_move(game_state& gs, std::uint64_t make_unmake) noexcept
{
    const std::uint32_t make   = make_unmake;
    const std::uint32_t unmake = make_unmake >> 32;

    details::unmake_move_impl(gs.bb, make, unmake, gs.hash, gs.eval);

    // Decrement the ply-counter.
    gs.bb.ply_counter--;
}
