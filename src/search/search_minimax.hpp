#pragma once

#include "search.hpp"

#include "evaluation/evaluation.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace search
{

namespace details
{

int search_mini(const bitboard& bb, std::size_t depth, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept;
int search_maxi(const bitboard& bb, std::size_t depth, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept;

inline int search_maxi(const bitboard& bb, std::size_t depth, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
{
    // It's a draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return 0;

    if (depth == 0)
        return eval(bb, args);

    int ret { std::numeric_limits<int>::min() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        if (stop_search) [[unlikely]]
            return ret;

        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        const int score { search_mini(next_position, depth-1, eval, move_buf.subspan(moves), args) };
        if (score > ret)
            ret = score;
    }

    return ret;
}

inline int search_mini(const bitboard& bb, std::size_t depth, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
{
    // It's a draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return 0;

    if (depth == 0)
        return eval(bb, args);

    int ret { std::numeric_limits<int>::max() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        if (stop_search) [[unlikely]]
            return ret;

        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        const int score { search_maxi(next_position, depth-1, eval, move_buf.subspan(moves), args) };
        if (score < ret)
            ret = score;
    }

    return ret;
}

}

inline int search_minimax(const bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval)
{
    return bb.is_black_to_play() ? details::search_mini(bb, depth, eval, move_buf, args_eval) : details::search_maxi(bb, depth, eval, move_buf, args_eval);
}

}