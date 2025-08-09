#pragma once

#include "evaluation/evaluation.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace search
{

namespace details
{

inline int search_negamax_recursive(const bitboard& bb, std::size_t depth, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, int colour, const void* args) noexcept
{
    if (depth == 0)
        return colour*eval(bb, args);

    int ret { -std::numeric_limits<int>::max() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        ret = std::max(ret, -search_negamax_recursive(next_position, depth-1, eval, move_buf.subspan(moves), -colour, args));
    }

    return ret;
}

}

// The args-search must contain a std::size_t target depth.
inline int search_negamax(const bitboard& bb, std::size_t max_depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval)
{
    const int colour = (bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_recursive(bb, max_depth, eval, move_buf, colour, args_eval);
}

}