#pragma once

#include "evaluation/evaluate_material.hpp"
#include "evaluation/evaluate_piece_square.hpp"
#include "evaluation/game_phase.hpp"
#include <limits>

namespace evaluation
{

constexpr int EVAL_CHECKMATE { std::numeric_limits<int>::max()/2 };

constexpr int evaluate_mg(const bitboard& bb) noexcept
{
    int ret {};

    for (std::size_t i = 0; i < 14; i++)
    {
        std::uint64_t piece_bb { bb.boards[i] };
        while (piece_bb)
        {
            const std::size_t mb = std::countr_zero(ls1b_isolate(piece_bb));

            ret += piece_mg_evaluation[i] + piece_square_mg_evaluation[i][mb];

            piece_bb = ls1b_reset(piece_bb);
        }
    }

    return ret;
}

constexpr int evaluate_eg(const bitboard& bb) noexcept
{
    int ret {};

    for (std::size_t i = 0; i < 14; i++)
    {
        std::uint64_t piece_bb { bb.boards[i] };
        while (piece_bb)
        {
            const std::size_t mb = std::countr_zero(ls1b_isolate(piece_bb));

            ret += piece_eg_evaluation[i] + piece_square_eg_evaluation[i][mb];

            piece_bb = ls1b_reset(piece_bb);
        }
    }

    return ret;
}

constexpr int evaluate(const bitboard& bb) noexcept
{
    return interpolate_gp(evaluate_mg(bb), evaluate_eg(bb), evaluate_gp(bb));
}

}