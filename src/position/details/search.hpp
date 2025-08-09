#pragma once

#include "position/search.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace details
{

int search_mini(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept;
int search_maxi(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept;

inline int search_maxi(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    if (depth == 0) return eval(bb);

    int ret { std::numeric_limits<int>::min() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move_buf[i])) [[unlikely]]
            continue;

        const int score { search_mini(next_position, depth-1, eval, move_buf.subspan(moves)) };
        if (score > ret)
            ret = score;
    }

    return ret;
}

inline int search_mini(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    if (depth == 0) return eval(bb);

    int ret { std::numeric_limits<int>::max() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move_buf[i])) [[unlikely]]
            continue;

        const int score { search_maxi(next_position, depth-1, eval, move_buf.subspan(moves)) };
        if (score < ret)
            ret = score;
    }

    return ret;
}

inline int evaluate_minimax_impl(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    return bb.is_black_to_play() ? details::search_mini(bb, depth, eval, move_buf) : details::search_maxi(bb, depth, eval, move_buf);
}

}

inline int evaluate_minimax(const bitboard& bb, std::size_t depth, evaluation eval) noexcept
{
    std::vector<std::uint32_t> move_buf(depth*MAX_MOVES_PER_POSITION);

    return details::evaluate_minimax_impl(bb, depth, eval, move_buf);
}

inline std::pair<std::uint32_t, int>best_move_minimax(const bitboard& bb, std::size_t depth, evaluation eval) noexcept
{
    std::vector<std::uint32_t> move_buf(depth*MAX_MOVES_PER_POSITION);
    std::span<std::uint32_t> move_span(move_buf);

    const bool is_black_to_play { bb.is_black_to_play() };
    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    std::pair<std::uint32_t, int> ret { 0, is_black_to_play ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min() };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        if (const int score { details::evaluate_minimax_impl(next_position, depth-1, eval, move_span.subspan(moves)) }; (is_black_to_play ? score < ret.second : score > ret.second) )
            ret = { move_buf[i], score };
    }

    return ret;
}