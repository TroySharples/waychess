#pragma once

#include "position/bitboard.hpp"

namespace evaluation
{

namespace details
{

constexpr std::array<int, 15> material_cp {
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
    -900,
    // Any.
    0
};

};

// Simply counts the material points - this is super basic and we don't expect it to be very good.
inline int evaluate_raw_material(const bitboard& bb) noexcept
{
    int ret {};

    // At the moment this is purely materialistic.
    for (std::uint8_t i = 0; i < 14; i++)
        ret += details::material_cp[i]*std::popcount(bb.boards[i]);

    return ret;
}

}
