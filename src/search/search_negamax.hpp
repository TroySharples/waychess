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

constexpr std::uint8_t META_EXACT    { 0 };
constexpr std::uint8_t META_ALL_NODE { 1 };
constexpr std::uint8_t META_CUT_NODE { 2 };

inline void sort_moves_pv(game_state& gs, std::span<std::uint32_t> move_buf) noexcept
{
    const auto draft = static_cast<std::uint8_t>(gs.bb.ply_counter-gs.root_ply);

    // Find the PV move (null if one doesn't exist).
    const std::uint32_t pv_move { gs.pv.lengths[draft] > 0 ? gs.pv.table[draft][0] : 0 };

    // If this is in our move list, bring it to the front.
    for (std::size_t i = 0; i < move_buf.size(); i++)
    {
        if (move_buf[i] == pv_move)
        {
            // Search this move first
            std::swap(move_buf[0], move_buf[i]);
            break;
        }
    }
}

inline int search_negamax_recursive(game_state& gs, statistics& stats, std::uint8_t depth, int a, int b, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    int ret { -std::numeric_limits<int>::max() };

    // Some constants that will be broadly useful throughout the search.
    const auto a_orig = a ;
    const auto draft = static_cast<std::uint8_t>(gs.bb.ply_counter-gs.root_ply);

    // Update stats.
    stats.nodes++;

    // Handle repetition-based draws first - we currently don't implement and contempt factor when playing against weaker opponents.
    // It is faster doing this here before the hash-lookup as in practice almost all hash-lookups will probably result in a cache-miss.
    if (gs.is_repetition_draw()) [[unlikely]]
        return 0;

    // If this is a leaf of our search tree we just use our terminal evaluation function. We have to multiply by the colour here
    // as in negamax our heuristic needs to be relative to the side-to-play of the node. We currently do this before the hash-lookup
    // to avoid possible cache-misses as our terminal evaluation function is cheap, but after we implement quiescent-search it will
    // probably be better to do this after the lookup.
    if (depth == 0)
        return colour*eval(gs.bb);

    // Loop up the value in the hash table.
    auto& entry { transposition_table[gs.hash] };
    const bool hash_hit { entry.key == gs.hash && entry.value.age == gs.age };

    // Only consider returning early if our hash entry has a higher depth (i.e. lower draft) than this current node.
    if (hash_hit && entry.value.depth >= depth)
    {
        if (entry.value.meta == META_EXACT
            || (entry.value.meta == META_CUT_NODE && entry.value.eval >= b)
            || (entry.value.meta == META_ALL_NODE && entry.value.eval <= a))
        return entry.value.eval;
    }

    // The actual evaluation bit.
    {
        // Otherwise, we need to continue the search by generating all nodes from here.
        const std::uint8_t moves { generate_pseudo_legal_moves(gs.bb, move_buf) };
        const std::span<std::uint32_t> move_list { move_buf.subspan(0, moves) };

        // Sort the moves favourably to increase the chance of early beta-cutoffs.
        sort_moves_pv(gs, move_list);

        for (const auto move : move_list)
        {
            // Allow us to break out of the search early if needed.
            if (stop_search) [[unlikely]]
                return ret;

            std::uint32_t unmake;
            if (!make_move({ .check_legality = true }, gs, move, unmake)) [[unlikely]]
            {
                // Simply unmake the move and move on to the next one if it's illegal (note that the move is still applied in make_move
                // even if it ends up being illegal).
                unmake_move(gs, unmake);
                continue;
            }

            // Recurse this algorithm, and make sure to unmake after the fact.
            ret = std::max(ret, -search_negamax_recursive(gs, stats, depth-1, -b, -a, -colour, eval, move_buf.subspan(moves)));
            unmake_move(gs, unmake);

            // Update our PV table when we've found a new best move at this depth.
            if (ret > a)
            {
                a = ret;
                gs.pv.update(draft, move);
            }

            // Break early if we encounter a beta-cutoff (fantastic news!).
            if (a >= b)
                break;
        }

        // Handle the rare case of stalemate where there are no legal moves in the position but we aren't in check. We detect this by
        // making seeing if we haven't updated our evaluation from the start.
        if (ret == -std::numeric_limits<int>::max() && !is_in_check(gs.bb, colour == -1)) [[unlikely]]
            ret = 0;
    }

    // Handle updating our transposition table. We currently employ the very simple strategy of always overwriting unless the other
    // entry recent (i.e. not from a previous search) and was at a higher depth.
    if ((entry.value.age != gs.age || entry.value.depth <= depth))
    {
        entry.key             = gs.hash;
        entry.value.age       = gs.age;
        entry.value.depth     = depth;
        entry.value.eval      = ret;
        entry.value.best_move = (ret > a_orig) ? gs.pv.table[draft][0] : 0;
        if (ret <= a_orig)
            entry.value.meta = META_ALL_NODE;
        else if (ret >= b)
            entry.value.meta = META_CUT_NODE;
        else
            entry.value.meta = META_EXACT;
    }

    return ret;
}

}

inline int search_negamax(game_state& gs, statistics& stats, std::uint8_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval)
{
    const int colour = (gs.bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_recursive(gs, stats, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, eval, move_buf);
}

}