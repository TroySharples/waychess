#pragma once


#include "evaluation/evaluate.hpp"
#include "evaluation/evaluate_material.hpp"
#include "evaluation/see.hpp"
#include "pieces/pieces.hpp"
#include "position/game_state.hpp"
#include "position/move.hpp"
#include "search/statistics.hpp"
#include "search/search_quiescent.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include "config.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace search
{

namespace details
{

constexpr std::uint8_t META_EXACT       { 0 };
constexpr std::uint8_t META_LOWER_BOUND { 1 };
constexpr std::uint8_t META_UPPER_BOUND { 2 };

inline int score_move(std::uint32_t move, std::size_t draft, const game_state& gs, std::uint32_t pv_move, std::uint32_t hash_move) noexcept
{
    // We score PV moves the highest.
    if (move == pv_move)
        return 2000000000;

    // Next comes hash-moves.
    if (move == hash_move)
        return 1500000000;

    // Next we score promotions according to the score of the promoted piece (we may change this in the future so that all non-queen
    // promotions are scored near the bottom).
    if (move & move::type::PROMOTION)
        return 1000000000 + std::abs(evaluation::piece_mg_evaluation[move::make_decode_promotion(move)]);

inline void score_move(std::uint64_t& move, std::size_t draft, const game_state& gs, std::uint32_t pv_move, std::uint32_t hash_move) noexcept
{
    int score;
    if (move::move_is_equal(move, pv_move))
    {
        score = 2000000000;
    }
    else if (move::move_is_equal(move, hash_move))
    {
        score = 1500000000;
    }
    else if (move & move::type::PROMOTION)
    {
        score = 1000000000 + std::abs(evaluation::piece_mg_evaluation[move::make_decode_promotion(move)]);
    }
    else if (move & move::type::CAPTURE)
    {
    // Score captures according to MVV/LVA score. Captures are divided into winning and loosing buckets according to their SEE, with winning
    // captures being defined as having a positive SEE with some delta-flexibility.
    score = 900000000 + mvv_lva_score(gs.bb, move);
    }
    else if (config::km && gs.km.is_killer_move(draft, move))
    {
        // move |= move::info::KILLER;
        score = 800000000;
    }
    else if (config::hh)
    {
        score = gs.hh.get_bonus(move);
    }
    else
    {
        score = 0;
    }

    // Set the score.
    move |= (static_cast<std::int64_t>(score) << 32);
}

inline void sort_moves(std::span<std::uint64_t> move_buf, std::size_t draft, const game_state& gs, std::uint64_t pv_move, std::uint64_t hash_move) noexcept
{
    // Score each move and fill-out the move-info.
    std::for_each(move_buf.begin(), move_buf.end(), [&gs, draft, pv_move, hash_move] (std::uint64_t& move) { score_move(move, draft, gs, pv_move, hash_move); });

    // Sort the moves (high-to-low).
    const std::span<std::int64_t> signed_move_buf { reinterpret_cast<std::int64_t*>(move_buf.data()), move_buf.size() };
    for (auto& move : signed_move_buf)
        move |= (static_cast<std::int64_t>(score_move(move, draft, gs, pv_move, hash_move)) << 32);

    // Sort the moves (high-to-low).
    std::sort(signed_move_buf.rbegin(), signed_move_buf.rend());
}

inline void handle_fail_high(game_state& gs, std::size_t draft, std::uint32_t move)
{
    // Handle quiet moves that fail-high.
    if (!(move & (move::type::PROMOTION | move::type::CAPTURE)))
    {
        if (config::km) gs.km.store_killer_move(draft, move);
        if (config::hh) gs.hh.update_hh(move);
    }
}

inline int search_negamax_recursive(game_state& gs, statistics& stats, std::size_t depth, int alpha, int beta, int colour, std::span<std::uint64_t> move_buf) noexcept
{
    int ret { -std::numeric_limits<int>::max() };

    // Some constants that will be broadly useful throughout the search.
    const auto alpha_orig = alpha ;
    const size_t draft { gs.bb.ply_counter-gs.root_ply };

    // Update stats.
    stats.abnodes++;

    // Handle repetition-based draws first - we currently don't implement and contempt factor when playing against weaker opponents.
    // It is faster doing this here before the hash-lookup as in practice almost all hash-lookups will probably result in a cache-miss.
    if (gs.is_repetition_draw()) [[unlikely]]
        return 0;

    // Loop up the value in the hash table.
    stats.tt_probes++;
    auto& entry { gs.tt[gs.hash] };
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
            stats.pvnodes++;
            if (entry.value.best_move)
                gs.pv.table[draft][0] = entry.value.best_move;
            return eval;
        }

        // All-node (fail-low). We have an upperbound for how good this move can be. If this is less than our alpha there's no point in
        // continuing this search.
        if (entry.value.meta == META_UPPER_BOUND && eval <= alpha)
        {
            stats.allnodes++;
            return eval;
        }

        // Cut-node (fail-high). We have a lowerbound for how good this move can be. If this is greater than our beta, there's no point
        // in trying to find a stronger refutation.
        if (entry.value.meta == META_LOWER_BOUND && eval >= beta)
        {
            stats.cutnodes++;
            stats.fh_hash++;
            if (const std::uint32_t best_move { entry.value.best_move }; best_move) handle_fail_high(gs, draft, best_move);
            return eval;
        }
    }

    // Loop up our PV and hash moves from previous searches.
    const std::uint32_t pv_move        { gs.pv.table[draft][0] };
    const std::uint32_t hash_move      { hash_hit ? entry.value.best_move : 0 };
    const bool in_check                { is_in_check(gs.bb, colour == -1) };
    const bool is_king_and_pawn_colour { gs.bb.is_king_and_pawn(colour == -1) };

    // The actual evaluation bit.
    std::uint32_t best_move {};
    if (depth == 0)
    {
        // If this is a leaf of our search tree we just use our quiescent-search function to mitigate the horizon-effect.
        ret = search_quiescence(gs, stats, 0, alpha, beta, colour, move_buf);
    }
    else
    {
        // See if we can perform null-move pruning - this relies on not being in check or a king-and-pawn endgame.
        const std::size_t r { depth > 6 ? 4ULL : 3ULL };
        bool null_move_pruned { false };
        if (config::nmp && !in_check && !is_king_and_pawn_colour && depth >= r+1)
        {
            stats.moves_all++;
            stats.moves_null++;

            // No need to check legality here as we already know this move won't leave us in check.
            std::uint32_t unmake;
            make_move({ .check_legality = false }, gs, move::NULL_MOVE, unmake);
            const int score { -search_negamax_recursive(gs, stats, depth-r-1, -beta, -beta+1, -colour, move_buf) };
            unmake_move(gs, move::NULL_MOVE, unmake);

            // Return early if this reduced search causes a beta-cutoff.
            if (score >= beta)
            {
                stats.fh_null++;
                null_move_pruned = true;
                ret = score;
            }
        }

        // Continue with the rest of our search if we were unable to prune any nodes.
        if (!null_move_pruned)
        {
            // Otherwise, we need to continue the search by generating all nodes from here.
            const std::size_t moves { generate_pseudo_legal_moves(gs.bb, move_buf) };
            const std::span<std::uint64_t> move_list { move_buf.subspan(0, moves) };

            // Sort the moves favourably to increase the chance of early beta-cutoffs.
            sort_moves(move_list, draft, gs, pv_move, hash_move);

            for (std::size_t i = 0; i < moves; i++)
            {
                stats.moves_all++;

                const std::uint64_t make { move_list[i] };

                // Allow us to break out of the search early if needed.
                if (gs.stop_search) [[unlikely]]
                    return ret;

                std::uint32_t unmake;
                if (!make_move({ .check_legality = true }, gs, make, unmake)) [[unlikely]]
                {
                    stats.moves_illegal++;
                    // Simply unmake the move and move on to the next one if it's illegal (note that the move is still applied in make_move
                    // even if it ends up being illegal).
                    unmake_move(gs, make, unmake);
                    continue;
                }

                // We run the recursive search at a lower depth if this move isn't near the top of our list after sorting. TODO: have smarter
                // adaptive LMR-reduction, and tweek the LMR kick-in.
                const bool do_lmr { config::lmr && i >= 3 && depth > 3 };
                constexpr std::size_t lmr_reduction { 1 };
                const std::size_t d { do_lmr ? depth-lmr_reduction-1 : depth-1 };
                if (do_lmr)
                    stats.moves_lmr++;

                const bool do_scout { config::scout && i >= 1 };
                const int b { do_scout ? alpha+1 : beta };
                if (do_scout)
                    stats.moves_pvs++;

                // Recurse negamax.
                int score = -search_negamax_recursive(gs, stats, d, -b, -alpha, -colour, move_buf.subspan(moves));

                // Handle researching at full-depth / widened window if necessary.
                if (do_scout && alpha < score && score < beta && depth > 0)
                {
                    stats.pvs_researches++;
                    score = -search_negamax_recursive(gs, stats, depth-1, -beta, -score, -colour, move_buf.subspan(moves));
                }
                else if (do_lmr && score > alpha)
                {
                    stats.lmr_researches++;
                    score = -search_negamax_recursive(gs, stats, depth-1, -beta, -alpha, -colour, move_buf.subspan(moves));
                }

                // Resets the game state to how it was before we made the move.
                unmake_move(gs, make, unmake);

                if (score > ret)
                {
                    best_move = make;
                    ret = score;
                }

                // Update our PV table when we've found a new best move at this depth.
                if (ret > alpha)
                {
                    stats.moves_improve++;

                    alpha = ret;
                    gs.pv.update_variation(draft, make);
                }

                // Break early if we encounter a beta-cutoff (fantastic news!).
                if (alpha >= beta)
                {
                    stats.moves_cut++;

                    // Update statistics on which move caused the cut.
                    i == 0 ? stats.fh_first++ : stats.fh_later++;
                    handle_fail_high(gs, draft, best_move);
                    break;
                }

                // Update the relative butterfly heuristic.
                if (config::hh && !(make & (move::type::PROMOTION | move::type::CAPTURE))) gs.hh.update_bf(make);
            }

            // Handle the rare case of there being no legal moves in this position. This should be evaluated as either checkmate (if we're in check) or as
            // a draw (if it is stalemate).
            if (!best_move) [[unlikely]]
                ret = is_in_check(gs.bb, colour == -1) ? -evaluation::EVAL_CHECKMATE : 0;
        }
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
        entry.value.best_move = (ret > alpha_orig ? best_move : 0);

        // Set our node-type.
        if (ret <= alpha_orig)
        {
            stats.allnodes++;
            entry.value.meta = META_UPPER_BOUND;
        }
        else if (ret >= beta)
        {
            stats.cutnodes++;
            entry.value.meta = META_LOWER_BOUND;
        }
        else
        {
            stats.pvnodes++;
            entry.value.meta = META_EXACT;
        }
    }

    return ret;
}

}

inline int search_negamax(game_state& gs, statistics& stats, std::size_t depth, int colour, std::span<std::uint64_t> move_buf, int d = config::awd) noexcept
{
    int a { -std::numeric_limits<int>::max() };
    int b {  std::numeric_limits<int>::max() };

    // Don't narrow the window if our initial delta is 0 - we just return the result of the search across the whole space.
    if (!d)
        return details::search_negamax_recursive(gs, stats, depth, a, b, colour, move_buf);

    // Set a narrower window around the previous score for this node if we can find it in the transposition table.
    if (const auto& entry { gs.tt[gs.hash] }; entry.key == gs.hash)
    {
        a = entry.value.eval-d;
        b = entry.value.eval+d;
    }

    while (true)
    {
        const int score { details::search_negamax_recursive(gs, stats, depth, a, b, colour, move_buf) };
        if (gs.stop_search)
            return score;

        // Widen the window back out to the maximum if we've found checkmate - not doing this can be very risky, and
        // can even end up in us recommending illegal moves!
        if (std::abs(score) >= evaluation::EVAL_CHECKMATE)
            return details::search_negamax_recursive(gs, stats, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, move_buf);

        // Otherwise check that it fits within the window, and adjust if necessary.
        if (score <= a)
        {
            stats.aw_misses_low++;
            a -= d;
        }
        else if (score >= b)
        {
            stats.aw_misses_high++;
            b += d;
        }
        else
        {
            return score;
        }

        d *= 2;
    }
}

}