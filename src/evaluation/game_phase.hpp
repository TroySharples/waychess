#pragma once

#include "position/bitboard.hpp"

namespace evaluation
{

namespace details
{

/*********************************************************************
* The game phase is a metric calculated by total on-board material   *
* indicating how far between the middle-game and end-game you are.   *
* Following PeSTO in RofChade, this is a score between 0 and 24 (0   *
* being fully in a king-and-pawn end-game), and is used to           *
* interpolate middle-game and end-game evaluations.                  *
*********************************************************************/

constexpr int pawn_gp   { 0 };
constexpr int knight_gp { 1 };
constexpr int bishop_gp { 1 };
constexpr int rook_gp   { 2 };
constexpr int queen_gp  { 4 };
constexpr int king_gp   { 0 };

}

constexpr std::array<int, 15> piece_gp {
    details::pawn_gp,
    details::king_gp,
    details::knight_gp,
    details::bishop_gp,
    details::rook_gp,
    details::queen_gp,
    // Gaps in piece ID.
    0, 0,
    details::pawn_gp,
    details::king_gp,
    details::knight_gp,
    details::bishop_gp,
    details::rook_gp,
    details::queen_gp,
    // Empty.
    0
};

constexpr int evaluate_gp(const bitboard& bb) noexcept
{
    int ret {};

    for (std::uint8_t i = 0; i < 14; i++)
        ret += piece_gp[i]*std::popcount(bb.boards[i]);

    return ret;
}

// Interpolates between a middle-game and end-game score.
constexpr int interpolate_gp(int mg, int eg, int gp) noexcept
{
    // Clamp gp at 24 (the starting value) to guard against any crazy high-material-value positions
    // resulting from a million promotions.
    gp = std::min(gp, 24);

    return (mg*gp + eg*(24-gp))/24;
}

}