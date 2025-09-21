#pragma once

#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"

namespace evaluation
{

namespace details
{

constexpr int isolated_pawns_penalty_mg { 10 };
constexpr int isolated_pawns_penalty_eg { 15 };

constexpr int doubled_pawns_penalty_mg { 8 };
constexpr int doubled_pawns_penalty_eg { 15 };

constexpr int blocked_pawns_penalty_mg { 12 };
constexpr int blocked_pawns_penalty_eg { 10 };

}

inline int evaluate_pawn_structure_mg(const bitboard& bb)
{
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    return std::popcount(get_pawn_isolated(bb.boards[b_pawn]))           * details::isolated_pawns_penalty_mg
         + std::popcount(get_pawn_doubled(bb.boards[b_pawn]))            * details::doubled_pawns_penalty_mg
         + std::popcount(get_black_pawn_blocked(bb.boards[b_pawn], occ)) * details::blocked_pawns_penalty_mg
         - std::popcount(get_pawn_isolated(bb.boards[w_pawn]))           * details::isolated_pawns_penalty_mg
         - std::popcount(get_pawn_doubled(bb.boards[w_pawn]))            * details::doubled_pawns_penalty_mg
         - std::popcount(get_white_pawn_blocked(bb.boards[w_pawn], occ)) * details::blocked_pawns_penalty_mg;
}

inline int evaluate_pawn_structure_eg(const bitboard& bb)
{
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    return std::popcount(get_pawn_isolated(bb.boards[b_pawn]))           * details::isolated_pawns_penalty_eg
         + std::popcount(get_pawn_doubled(bb.boards[b_pawn]))            * details::doubled_pawns_penalty_eg
         + std::popcount(get_black_pawn_blocked(bb.boards[b_pawn], occ)) * details::blocked_pawns_penalty_eg
         - std::popcount(get_pawn_isolated(bb.boards[w_pawn]))           * details::isolated_pawns_penalty_eg
         - std::popcount(get_pawn_doubled(bb.boards[w_pawn]))            * details::doubled_pawns_penalty_eg
         - std::popcount(get_white_pawn_blocked(bb.boards[w_pawn], occ)) * details::blocked_pawns_penalty_eg;
}

}