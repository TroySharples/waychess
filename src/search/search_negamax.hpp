#pragma once

#include "evaluation/evaluation.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace search
{

namespace details
{

inline int search_negamax_recursive(const bitboard& bb, std::size_t depth, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
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

        ret = std::max(ret, -search_negamax_recursive(next_position, depth-1, -colour, eval, move_buf.subspan(moves), args));
    }

    return ret;
}

inline int search_negamax_prune_recursive(const bitboard& bb, std::size_t depth, int a, int b, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
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

        ret = std::max(ret, -search_negamax_prune_recursive(next_position, depth-1, -b, -a, -colour, eval, move_buf.subspan(moves), args));

        a = std::max(a, ret);
        if (a >= b)
            break;
    }

    return ret;
}

}

inline int search_negamax(const bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval)
{
    const int colour = (bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_recursive(bb, depth, colour, eval, move_buf, args_eval);
}

inline int search_negamax_prune(const bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval)
{
    const int colour = (bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_prune_recursive(bb, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, eval, move_buf, args_eval);
}

}