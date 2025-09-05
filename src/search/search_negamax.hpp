#pragma once

#include "evaluation/evaluation.hpp"

#include "search/statistics.hpp"
#include "search/transposition_table.hpp"
#include "search/search_quiescent.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace search
{

namespace details
{

constexpr std::uint8_t META_EXACT       { 0 };
constexpr std::uint8_t META_LOWER_BOUND { 1 };
constexpr std::uint8_t META_UPPER_BOUND { 2 };

inline void sort_moves(std::span<std::uint32_t> move_buf, std::uint32_t move) noexcept
{
    // If this is in our move list, bring it to the front.
    for (std::size_t i = 0; i < move_buf.size(); i++)
    {
        if (move_buf[i] == move)
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

    // Loop up the value in the hash table.
    stats.tt_probes++;
    auto& entry { transposition_table[gs.hash] };
    const bool hash_hit { entry.key == gs.hash };
    if (hash_hit)
        stats.tt_hits++;

    // Only consider returning early if our hash entry is the right age (i.e. is from this search) and has a higher depth (i.e. lower
    // draft) than this current node.
    if (hash_hit && entry.value.age == gs.age && entry.value.depth >= depth)
    {
        const int eval { entry.value.eval };

        // PV-node.
        if (entry.value.meta == META_EXACT)
        {
            // We have an exact score - lucky us! We might be able to retrieve a PV from it as well!
            if (entry.value.best_move)
                gs.pv.table[draft][0] = entry.value.best_move;
            return eval;
        }

        // All-node (fail-low). We have an upperbound for how good this move can be. If this is less than our alpha there's no point in
        // continuing this search.
        if (entry.value.meta == META_UPPER_BOUND && eval <= a)
            return eval;

        // Cut-node (fail-high). We have a lowerbound for how good this move can be. If this is greater than our beta, there's no point
        // in trying to find a stronger refutation.
        if (entry.value.meta == META_LOWER_BOUND && eval >= b)
            return eval;
    }

    // Loop up our PV and hash moves from previous searches.
    const std::uint32_t pv_move   { gs.pv.table[draft][0] };
    const std::uint32_t hash_move { hash_hit ? entry.value.best_move : 0 };

    // The actual evaluation bit.
    std::uint32_t best_move {};
    if (depth == 0)
    {
        // If this is a leaf of our search tree we just use our quiescent-search function to mitigate the horizon-effect.
        ret = search_quiescence(gs, stats, a, b, colour, eval, move_buf);
    }
    else
    {
        // Otherwise, we need to continue the search by generating all nodes from here.
        const std::uint8_t moves { generate_pseudo_legal_moves(gs.bb, move_buf) };
        const std::span<std::uint32_t> move_list { move_buf.subspan(0, moves) };

        // Sort the moves favourably to increase the chance of early beta-cutoffs. We're happy to use hints from previous searches here.
        if (hash_move)
            sort_moves(move_list, hash_move);
        if (pv_move)
            sort_moves(move_list, pv_move);

        for (const auto move : move_list)
        {
            // Allow us to break out of the search early if needed.
            if (gs.stop_search) [[unlikely]]
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
            const int score { -search_negamax_recursive(gs, stats, depth-1, -b, -a, -colour, eval, move_buf.subspan(moves)) };
            unmake_move(gs, unmake);
            if (score > ret)
            {
                best_move = move;
                ret = score;
            }

            // Update our PV table when we've found a new best move at this depth.
            if (ret > a)
            {
                a = ret;
                gs.pv.update_variation(draft, move);
            }

            // Break early if we encounter a beta-cutoff (fantastic news!).
            if (a >= b)
                break;
        }

        // Handle the rare case of there being no legal moves in this position, but us not being in check (i.e. stalemate).
        if (!best_move && !is_in_check(gs.bb, colour == -1)) [[unlikely]]
            ret = 0;
    }

    // Handle updating our transposition table. We currently employ the very simple strategy of always overwriting unless the other
    // entry recent (i.e. not from a previous search) and was at a higher depth.
    if (!hash_hit || entry.value.age != gs.age || entry.value.depth <= depth)
    {
        // Set basic parameters.
        entry.key         = gs.hash;
        entry.value.age   = gs.age;
        entry.value.depth = depth;
        entry.value.eval  = ret;

        // Set the best move if we've at least raised our alpha.
        entry.value.best_move = (ret > a_orig ? best_move : 0);

        // Set our node-type.
        if (ret <= a_orig)
            entry.value.meta = META_UPPER_BOUND;
        else if (ret >= b)
            entry.value.meta = META_LOWER_BOUND;
        else
            entry.value.meta = META_EXACT;
    }

    return ret;
}

inline int search_negamax_aspiration_window_recursive(game_state& gs, statistics& stats, std::uint8_t depth, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    // TODO: Narrow this after improving our evaluation function.
    constexpr int initial_delta { 201 };

    int d = initial_delta;
    int a = gs.last_score*colour-d;
    int b = gs.last_score*colour+d;

    while (true)
    {
        const int score { details::search_negamax_recursive(gs, stats, depth, a, b, colour, eval, move_buf) };
        if (gs.stop_search)
            return score;

        // We return if we hit checkmate.
        if (std::abs(score) == std::numeric_limits<int>::max())
            return score;

        // Otherwise check that it fits within the window - adjust otherwise!
        if (score <= a)
            a -= d;
        else if (score >= b)
            b += d;
        else
            return score;

        d *= 2;
    }
}

}

inline int search_negamax(game_state& gs, statistics& stats, std::uint8_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval)
{
    const int colour = (gs.bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_recursive(gs, stats, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, eval, move_buf);
}

inline int search_negamax_aspiration_window(game_state& gs, statistics& stats, std::uint8_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval)
{
    const int colour = (gs.bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax_aspiration_window_recursive(gs, stats, depth, colour, eval, move_buf);
}

}