#pragma once

#include <array>

namespace evaluation
{

namespace details
{

/*********************************************************************
* PeSTO's raw piece evaluation used in RofChade.                     *
*********************************************************************/

constexpr int white_pawn_mg_evaluation   { 82 };
constexpr int white_pawn_eg_evaluation   { 94 };
constexpr int white_knight_mg_evaluation { 337 };
constexpr int white_knight_eg_evaluation { 281 };
constexpr int white_bishop_mg_evaluation { 365 };
constexpr int white_bishop_eg_evaluation { 297 };
constexpr int white_rook_mg_evaluation   { 477 };
constexpr int white_rook_eg_evaluation   { 512 };
constexpr int white_queen_mg_evaluation  { 1025 };
constexpr int white_queen_eg_evaluation  { 936 };
constexpr int white_king_mg_evaluation   { 10000 };
constexpr int white_king_eg_evaluation   { 10000 };

constexpr int black_pawn_mg_evaluation   { -white_pawn_mg_evaluation };
constexpr int black_pawn_eg_evaluation   { -white_pawn_eg_evaluation };
constexpr int black_knight_mg_evaluation { -white_knight_mg_evaluation };
constexpr int black_knight_eg_evaluation { -white_knight_eg_evaluation };
constexpr int black_bishop_mg_evaluation { -white_bishop_mg_evaluation };
constexpr int black_bishop_eg_evaluation { -white_bishop_eg_evaluation };
constexpr int black_rook_mg_evaluation   { -white_rook_mg_evaluation };
constexpr int black_rook_eg_evaluation   { -white_rook_eg_evaluation };
constexpr int black_queen_mg_evaluation  { -white_queen_mg_evaluation };
constexpr int black_queen_eg_evaluation  { -white_queen_eg_evaluation };
constexpr int black_king_mg_evaluation   { -white_king_mg_evaluation };
constexpr int black_king_eg_evaluation   { -white_king_eg_evaluation };

};

constexpr std::array<int, 15> piece_mg_evaluation {
    details::white_pawn_mg_evaluation,
    details::white_king_mg_evaluation,
    details::white_knight_mg_evaluation,
    details::white_bishop_mg_evaluation,
    details::white_rook_mg_evaluation,
    details::white_queen_mg_evaluation,
    // Gaps in piece ID.
    0, 0,
    details::black_pawn_mg_evaluation,
    details::black_king_mg_evaluation,
    details::black_knight_mg_evaluation,
    details::black_bishop_mg_evaluation,
    details::black_rook_mg_evaluation,
    details::black_queen_mg_evaluation,
    // Empty.
    0
};

constexpr std::array<int, 15> piece_eg_evaluation {
    details::white_pawn_eg_evaluation,
    details::white_king_eg_evaluation,
    details::white_knight_eg_evaluation,
    details::white_bishop_eg_evaluation,
    details::white_rook_eg_evaluation,
    details::white_queen_eg_evaluation,
    // Gaps in piece ID.
    0, 0,
    details::black_pawn_eg_evaluation,
    details::black_king_eg_evaluation,
    details::black_knight_eg_evaluation,
    details::black_bishop_eg_evaluation,
    details::black_rook_eg_evaluation,
    details::black_queen_eg_evaluation,
    // Empty.
    0
};

}
