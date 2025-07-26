#pragma once

#include "position/evaluate.hpp"

namespace details
{

constexpr std::array<std::int16_t, 14> material_cp {
    // White pawn.
    100,
    // White king.
    10000,
    // White knight.
    300,
    // White bishop.
    320,
    // White rook,
    500,
    // White queen.
    900,
    // Gaps in piece ID.
    0, 0,
    // Black pawn.
    -100,
    // Black king.
    -10000,
    // Black knight.
    -300,
    // Black bishop.
    -320,
    // Black rook.
    -500,
    // Black queen.
    -900
};

};

inline std::int16_t evaluate_terminal(const bitboard& bb) noexcept
{
    std::int16_t ret {};

    // At the moment his is purely materialistic.
    for (std::size_t i = 0; i < 14; i++)
        ret += details::material_cp[i]*static_cast<std::int16_t>(std::popcount(bb.boards[i]));

    return ret;
}
