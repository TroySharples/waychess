#pragma once

#include "position/search.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace details
{

std::int16_t search_mini(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept;
std::int16_t search_maxi(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept;

inline std::int16_t search_maxi(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    if (depth == 0) return eval(bb);

    std::int16_t ret { std::numeric_limits<std::int16_t>::min() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move_buf[i])) [[unlikely]]
            continue;

        const std::int16_t score { search_mini(next_position, depth-1, eval, move_buf.subspan(moves)) };
        if (score > ret)
            ret = score;
    }

    return ret;
}

inline std::int16_t search_mini(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    if (depth == 0) return eval(bb);

    std::int16_t ret { std::numeric_limits<std::int16_t>::max() };

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move_buf[i])) [[unlikely]]
            continue;

        const std::int16_t score { search_maxi(next_position, depth-1, eval, move_buf.subspan(moves)) };
        if (score < ret)
            ret = score;
    }

    return ret;
}

inline std::int16_t evaluate_minimax_impl(const bitboard& bb, std::size_t depth, evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    return bb.is_black_to_play() ? details::search_mini(bb, depth, eval, move_buf) : details::search_maxi(bb, depth, eval, move_buf);
}

}

inline std::int16_t evaluate_minimax(const bitboard& bb, std::size_t depth, evaluation eval) noexcept
{
    constexpr std::size_t max_moves_per_position { 218 };
    std::vector<std::uint32_t> move_buf(depth*max_moves_per_position);

    return details::evaluate_minimax_impl(bb, depth, eval, move_buf);
}

inline std::pair<std::uint32_t, std::int16_t>best_move_minimax(const bitboard& bb, std::size_t depth, evaluation eval) noexcept
{
    constexpr std::size_t max_moves_per_position { 218 };
    std::vector<std::uint32_t> move_buf(depth*max_moves_per_position);
    std::span<std::uint32_t> move_span(move_buf);

    const bool is_black_to_play { bb.is_black_to_play() };
    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    std::pair<std::uint32_t, std::int16_t> ret { 0, is_black_to_play ? std::numeric_limits<std::int16_t>::max() : std::numeric_limits<std::int16_t>::min() };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        if (const std::int16_t score { details::evaluate_minimax_impl(next_position, depth-1, eval, move_span.subspan(moves)) }; (is_black_to_play ? score < ret.second : score > ret.second) )
            ret = { move_buf[i], score };
    }

    return ret;
}