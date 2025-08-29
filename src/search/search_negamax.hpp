#pragma once

#include "search.hpp"

#include "evaluation/evaluation.hpp"

#include "search/transposition_table.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace search
{

namespace details
{

constexpr std::uint8_t META_EXACT      { 0 };
constexpr std::uint8_t META_UPPERBOUND { 1 };
constexpr std::uint8_t META_LOWERBOUND { 2 };

inline int search_negamax_recursive(game_state& gs, std::uint8_t depth, int a, int b, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    // Return draw-score immediately if this position can claim a repetition / 50-move-rule based draw).
    if (gs.is_repetition_draw())
        return 0;

    if (depth == 0)
        return colour*eval(gs.bb);

    // Loop up the value in the hash table and return immediately if we hit. Note that we implement hash-ageing based on the root search node.
    auto& entry { transposition_table[gs.hash] };
    if (entry.key == gs.hash && entry.value.age == gs.age && entry.value.depth >= depth)
    {
        if (entry.value.meta == META_EXACT
            || (entry.value.meta == META_LOWERBOUND && entry.value.eval >= b)
            || (entry.value.meta == META_UPPERBOUND && entry.value.eval <= a))
        return entry.value.eval;
    }

    int ret { -std::numeric_limits<int>::max() };

    const std::uint8_t moves { generate_pseudo_legal_moves(gs.bb, move_buf) };

    const int a_orig { a };
    for (std::uint8_t i = 0; i < moves; i++)
    {
        if (stop_search) [[unlikely]]
            return ret;

        std::uint32_t unmake;
        if (!make_move({ .check_legality = true }, gs, move_buf[i], unmake)) [[unlikely]]
        {
            unmake_move(gs, unmake);
            continue;
        }

        ret = std::max(ret, -search_negamax_recursive(gs, depth-1, -b, -a, -colour, eval, move_buf.subspan(moves)));
        unmake_move(gs, unmake);

        a = std::max(a, ret);
        if (a >= b)
            break;
    }

    // Handle the rare case of stalemate where there are no legal moves in the position but we aren't in
    // check.
    if (ret == -std::numeric_limits<int>::max() && !is_in_check(gs.bb, colour == -1)) [[unlikely]]
        ret = 0;

    // Update the hash table with our result - always overriding for now.
    entry.key         = gs.hash;
    entry.value.age   = gs.age;
    entry.value.depth = depth;
    entry.value.eval  = ret;
    if (ret <= a_orig)
        entry.value.meta = META_UPPERBOUND;
    else if (ret >= b)
        entry.value.meta = META_LOWERBOUND;
    else
        entry.value.meta = META_EXACT;

    return ret;
}

}

inline int search_negamax(game_state& gs, std::uint8_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval)
{
    const int colour = (gs.bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_recursive(gs, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, eval, move_buf);
}

}