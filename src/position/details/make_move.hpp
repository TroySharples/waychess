#pragma once

#include "position/make_move.hpp"

#include "position/move.hpp"
#include "position/position.hpp"
#include "position/zobrist_hash.hpp"

namespace details
{

constexpr inline bool make_move_impl(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake, std::uint64_t& hash) noexcept
{
    bool ret { true };

    // We start with unmake the same as the forward direction. We might have to mask off the move-info later on
    // if this ends up being a capture.
    unmake = move;

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

    // Either do or undo black as the side to move.
    hash ^= zobrist::CODE_IS_BLACK_TO_MOVE;

    // Fill out the unmake en-passent / 50m-ply information and undo the zobrist en-passemnt.
    if (bb.en_passent_bb) [[unlikely]]
    {
        const std::uint8_t en_passent_mb { static_cast<std::uint8_t>(std::countr_zero(bb.en_passent_bb)) };

        unmake |= move::serialise_unmake_en_passent(en_passent_mb);
        hash   ^= zobrist::get_code_en_passent(en_passent_mb);

        bb.en_passent_bb = 0;
    }
    else
    {
        unmake |= move::serialise_unmake_ply_50m(bb.ply_50m);
    }

    // Increment the ply counter.
    bb.ply_counter++;

    // Handle the removal of castling rights. We use the from-to bitboard instead of just the from one as we also have
    // to remember to remove castling rights in the event one of our rooks gets captured - this is actually quite subtle...
    unmake |= move::serialise_unmake_castling(bb.castling);
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
    to_move_pieces ^= from_to_bb;
    hash ^= zobrist::get_code_piece(piece, from_mb);
    if (move::move_type::is_promotion(type)) [[unlikely]]
    {
        const piece_idx promote_idx { static_cast<piece_idx>(info | (is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn)) };

        bb.boards[piece] ^= from_bb;
        bb.boards[info | (is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn)] ^= to_bb;

        hash ^= zobrist::get_code_piece(promote_idx, to_mb);
    }
    else
    {
        bb.boards[piece] ^= from_to_bb;
        hash             ^= zobrist::get_code_piece(piece, to_mb);
    }

    // Handle piece captures.
    if (move::move_type::is_capture(type))
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };

        // En-passent captures are special - the piece to be taken off the board isn't the target move square.
        if (move::move_type::is_en_passent(type)) [[unlikely]]
        {
            const std::uint64_t capture_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);
            const std::uint64_t capture_mb = std::countr_zero(capture_bb);
            const piece_idx capture_idx { static_cast<piece_idx>(is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn) };

            opponent_pieces ^= capture_bb;
            bb.boards[capture_idx] ^= capture_bb;

            hash ^= zobrist::get_code_piece(capture_idx, capture_mb);
        }
        // For regular piece captures, we have to loop through the opponents piece bitboards to find the one containing
        // the piece to remove. We also have to remember to fill in the unmake-move so we can recover the piece when
        // going in reverse.
        else
        {
            opponent_pieces ^= to_bb;
            for (std::uint8_t capture_idx = (is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn); capture_idx <= (is_black_to_play ? piece_idx::w_queen : piece_idx::b_queen); capture_idx++)
            {
                if (std::uint64_t& capture_bitboard = bb.boards[capture_idx]; capture_bitboard & to_bb)
                {
                    capture_bitboard ^= to_bb;

                    unmake &= ~move::serialise_move_info(0x07);
                    unmake |= move::serialise_move_info(capture_idx);

                    hash ^= zobrist::get_code_piece(static_cast<piece_idx>(capture_idx), to_mb);

                    break;
                }
            }
        }

        bb.ply_50m = 0;
    }
    // If the move was just a pawn push we need to reset the ply counter for the 50 move rule.
    else if (move::move_type::is_pawn_push(type))
    {
        bb.ply_50m = 0;

        // If the move was a double pawn push we have to update the en-passent target square for the next move.
        if (info == move::move_info::PAWN_PUSH_DOUBLE)
        {
            bb.en_passent_bb = is_black_to_play ? (to_bb << 8) : (to_bb >> 8);
            hash ^= zobrist::get_code_en_passent(std::countr_zero(bb.en_passent_bb));
        }
    }
    // Otherwise we can increment the counter towards the 50 move limit.
    else
    {
        bb.ply_50m++;
    }

    // Handle moving the rook for castling (legality is checked below).
    if (move::move_type::is_castle(type)) [[unlikely]]
    {
        if (info == move::move_info::CASTLE_KS)
        {
            if (is_black_to_play)
            {
                constexpr std::uint64_t from_to_bb { (FILE_H & RANK_8) | (FILE_F & RANK_8) };
                bb.boards[piece_idx::b_rook] ^= from_to_bb;
                bb.boards[piece_idx::b_any]  ^= from_to_bb;

                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_H & RANK_8))
                  ^ zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_F & RANK_8))
                };
                hash ^= code;
            }
            else
            {
                constexpr std::uint64_t from_to_bb { (FILE_H & RANK_1) | (FILE_F & RANK_1) };
                bb.boards[piece_idx::w_rook] ^= from_to_bb;
                bb.boards[piece_idx::w_any]  ^= from_to_bb;

                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_H & RANK_1))
                  ^ zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_F & RANK_1))
                };
                hash ^= code;
            }
        }
        else
        {
            if (is_black_to_play)
            {
                constexpr std::uint64_t from_to_bb { (FILE_A & RANK_8) | (FILE_D & RANK_8) };
                bb.boards[piece_idx::b_rook] ^= from_to_bb;
                bb.boards[piece_idx::b_any]  ^= from_to_bb;

                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_A & RANK_8))
                  ^ zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_D & RANK_8))
                };
                hash ^= code;
            }
            else
            {
                constexpr std::uint64_t from_to_bb { (FILE_A & RANK_1) | (FILE_D & RANK_1) };
                bb.boards[piece_idx::w_rook] ^= from_to_bb;
                bb.boards[piece_idx::w_any]  ^= from_to_bb;

                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_A & RANK_1))
                  ^ zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_D & RANK_1))
                };
                hash ^= code;
            }
        }
    }

    // Handle left-in-check legality checking.
    if (args.check_legality)
    {
        if (move::move_type::is_castle(type)) [[unlikely]]
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
                ret = !get_attackers_white(bb, std::countr_zero(bb.boards[piece_idx::b_king]));
            else
                ret = !get_attackers_black(bb, std::countr_zero(bb.boards[piece_idx::w_king]));
        }
    }

    return ret;
}

inline void unmake_move_impl(bitboard& bb, std::uint32_t& unmake, std::uint64_t& hash) noexcept
{
    const bool is_black_to_play   { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces { is_black_to_play ? bb.boards[piece_idx::w_any] : bb.boards[piece_idx::b_any] };

    // The move only contains the piece up to colour - exactly which colour piece must be derived from the side-to-play.
    const std::uint8_t from_mb  { move::deserialise_from_mb(unmake) };
    const std::uint8_t to_mb    { move::deserialise_to_mb(unmake) };
    const piece_idx piece       { static_cast<piece_idx>(move::deserialise_piece_idx(unmake) | (is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn)) };
    const std::uint8_t type     { move::deserialise_move_type(unmake) };
    const std::uint8_t info     { move::deserialise_move_info(unmake) };
    const std::uint8_t castling { move::deserialise_unmake_castling(unmake) };
    const std::uint8_t ply_50m  { move::deserialise_unmake_ply_50m(unmake) };

    const std::uint64_t from_bb    { 1ULL << from_mb };
    const std::uint64_t to_bb      { 1ULL << to_mb };
    const std::uint64_t from_to_bb { from_bb | to_bb };

    // Either do or undo black as the side to move.
    hash ^= zobrist::CODE_IS_BLACK_TO_MOVE;

    // Decrement the ply counter.
    bb.ply_counter--;

    // Undo any current en-passent squares from the Zobrist hash.
    if (bb.en_passent_bb)
        hash ^= zobrist::get_code_en_passent(std::countr_zero(bb.en_passent_bb));

    // Handle en-passent / 50m-ply re-establishment.
    if ((ply_50m & 0x78) == 0x78) [[unlikely]]
    {
        const std::uint8_t en_passent_mb { (ply_50m & 0x07) | (is_black_to_play ? 0b101000 : 0b010000) };

        hash ^= zobrist::get_code_en_passent(en_passent_mb);

        bb.en_passent_bb = 1ULL << en_passent_mb;
        bb.ply_50m       = 0;
    }
    else
    {
        bb.en_passent_bb = 0;
        bb.ply_50m       = ply_50m;
    }

    // Set castling rights.
    if (bb.castling != castling) [[unlikely]]
    {
        hash ^= zobrist::get_code_castling(bb.castling) ^ zobrist::get_code_castling(castling);

        bb.castling = castling;
    }

    // Handle mechanically moving the pieces (from the to-square to the from-square).
    to_move_pieces ^= from_to_bb;
    hash ^= zobrist::get_code_piece(piece, from_mb);
    if (move::move_type::is_promotion(type)) [[unlikely]]
    {
        bb.boards[piece] ^= from_bb;

        // We have to loop through the bitboards to decide which piece to remove - we unfortunately can't store this in the
        // move-info in the case of capture-promotions.
        for (std::uint8_t promotion_idx = (is_black_to_play ? piece_idx::w_knight : piece_idx::b_knight); promotion_idx <= (is_black_to_play ? piece_idx::w_queen : piece_idx::b_queen); promotion_idx++)
        {
            if (std::uint64_t& promotion_bitboard = bb.boards[promotion_idx]; promotion_bitboard & to_bb)
            {
                hash ^= zobrist::get_code_piece(static_cast<piece_idx>(promotion_idx), to_mb);
                promotion_bitboard ^= to_bb;
                break;
            }
        }
    }
    else
    {
        hash ^= zobrist::get_code_piece(piece, to_mb);
        bb.boards[piece] ^= from_to_bb;
    }

    // Handle piece captures.
    if (move::move_type::is_capture(type))
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.boards[piece_idx::b_any] : bb.boards[piece_idx::w_any] };

        // En-passent captures are special - the piece to be taken off the board isn't the target move square.
        if (move::move_type::is_en_passent(type)) [[unlikely]]
        {
            const std::uint64_t capture_bb = is_black_to_play ? (to_bb >> 8) : (to_bb << 8);
            const std::uint64_t capture_mb = std::countr_zero(capture_bb);
            const piece_idx capture_idx { static_cast<piece_idx>(is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn) };

            hash ^= zobrist::get_code_piece(capture_idx, capture_mb);

            opponent_pieces ^= capture_bb;
            bb.boards[capture_idx] ^= capture_bb;
        }
        // For regular piece captures, we have to loop through the opponents piece bitboards to find the one containing
        // the piece to remove. We also have to remember to fill in the unmake-move so we can recover the piece when
        // going in reverse.
        else
        {
            const std::uint64_t capture_mb = std::countr_zero(to_bb);
            const piece_idx capture_idx { info | (is_black_to_play ? piece_idx::b_pawn : piece_idx::w_pawn) };

            hash ^= zobrist::get_code_piece(capture_idx, capture_mb);

            opponent_pieces ^= to_bb;
            bb.boards[capture_idx] ^= to_bb;
        }
    }

    // Handle moving the rook for castling.
    if (move::move_type::is_castle(type)) [[unlikely]]
    {
        if (info == move::move_info::CASTLE_KS)
        {
            if (is_black_to_play)
            {
                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_H & RANK_1))
                  ^ zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_F & RANK_1))
                };
                hash ^= code;

                constexpr std::uint64_t from_to_bb { (FILE_H & RANK_1) | (FILE_F & RANK_1) };
                bb.boards[piece_idx::w_rook] ^= from_to_bb;
                bb.boards[piece_idx::w_any]  ^= from_to_bb;
            }
            else
            {
                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_H & RANK_8))
                  ^ zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_F & RANK_8))
                };
                hash ^= code;

                constexpr std::uint64_t from_to_bb { (FILE_H & RANK_8) | (FILE_F & RANK_8) };
                bb.boards[piece_idx::b_rook] ^= from_to_bb;
                bb.boards[piece_idx::b_any]  ^= from_to_bb;
            }
        }
        else
        {
            if (is_black_to_play)
            {
                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_A & RANK_1))
                  ^ zobrist::get_code_piece(piece_idx::w_rook, std::countr_zero(FILE_D & RANK_1))
                };
                hash ^= code;

                constexpr std::uint64_t from_to_bb { (FILE_A & RANK_1) | (FILE_D & RANK_1) };
                bb.boards[piece_idx::w_rook] ^= from_to_bb;
                bb.boards[piece_idx::w_any]  ^= from_to_bb;
            }
            else
            {
                constexpr std::uint64_t code {
                    zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_A & RANK_8))
                  ^ zobrist::get_code_piece(piece_idx::b_rook, std::countr_zero(FILE_D & RANK_8))
                };
                hash ^= code;

                constexpr std::uint64_t from_to_bb { (FILE_A & RANK_8) | (FILE_D & RANK_8) };
                bb.boards[piece_idx::b_rook] ^= from_to_bb;
                bb.boards[piece_idx::b_any]  ^= from_to_bb;
            }
        }
    }
}

}

inline bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move) noexcept
{
    std::uint32_t unmake;
    std::uint64_t hash;
    return details::make_move_impl(args, bb, move, unmake, hash);
}

inline bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake) noexcept
{
    std::uint64_t hash;
    return details::make_move_impl(args, bb, move, unmake, hash);
}

inline bool make_move(const make_move_args& args, bitboard& bb, std::uint32_t move, std::uint32_t& unmake, std::uint64_t& hash) noexcept
{
    return details::make_move_impl(args, bb, move, unmake, hash);
}

inline void unmake_move(bitboard& bb, std::uint32_t& unmake) noexcept
{
    std::uint64_t hash;
    details::unmake_move_impl(bb, unmake, hash);
}

inline void unmake_move(bitboard& bb, std::uint32_t& unmake, std::uint64_t& hash) noexcept
{
    details::unmake_move_impl(bb, unmake, hash);
}
