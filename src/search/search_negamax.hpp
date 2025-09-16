#pragma once


#include "evaluation/evaluate.hpp"
#include "position/game_state.hpp"
#include "position/move.hpp"
#include "search/statistics.hpp"
#include "search/search_quiescent.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include "config.hpp"

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
        return 1000000000;

    // Next MVV/LVA score.
    if (move::move_type::is_capture(move::deserialise_move_type(move)))
        return 900000000 + mvv_lva_score(gs.bb, move);

    // Next is killer moves from this ply.
    if (gs.km.is_killer_move(draft, move))
        return 800000000;

    // Else just look the value using the history heuristic.
    if (config::hh) return gs.hh.get_bonus(move);

    return 0;
}

inline void sort_moves(std::span<std::uint32_t> move_buf, std::uint32_t move)
{
    for (std::size_t i = 0; i < move_buf.size(); i++)
    {
        if (move_buf[i] == move)
        {
            std::swap(move_buf[0], move_buf[i]);
            break;
        }
    }
}

inline void sort_moves(std::span<std::uint32_t> move_buf, std::size_t draft, const game_state& gs, std::uint32_t pv_move, std::uint32_t hash_move) noexcept
{
    std::array<std::pair<std::uint32_t, int>, MAX_MOVES_PER_POSITION> scored_moves;
    for (std::size_t i = 0; i < move_buf.size(); i++)
        scored_moves[i] = { move_buf[i], score_move(move_buf[i], draft, gs, pv_move, hash_move) };

    std::sort(scored_moves.begin(), scored_moves.begin() + move_buf.size(), [](const auto& a, const auto& b) noexcept { return a.second > b.second; });

    for (std::size_t i = 0; i < move_buf.size(); i++)
        move_buf[i] = scored_moves[i].first;
}

inline int search_negamax_recursive(game_state& gs, statistics& stats, std::size_t depth, int a, int b, int colour, std::span<std::uint32_t> move_buf) noexcept
{
    int ret { -std::numeric_limits<int>::max() };

    // Some constants that will be broadly useful throughout the search.
    const auto a_orig = a ;
    const size_t draft { gs.bb.ply_counter-gs.root_ply };

    // Update stats.
    stats.pvnodes++;

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
    const std::uint32_t pv_move        { gs.pv.table[draft][0] };
    const std::uint32_t hash_move      { hash_hit ? entry.value.best_move : 0 };
    const bool in_check                { is_in_check(gs.bb, colour == -1) };
    const bool is_king_and_pawn_colour { gs.bb.is_king_and_pawn(colour == -1) };

    // The actual evaluation bit.
    std::uint32_t best_move {};
    if (depth == 0)
    {
        // If this is a leaf of our search tree we just use our quiescent-search function to mitigate the horizon-effect.
        ret = search_quiescence(gs, stats, 0, a, b, colour, move_buf);
    }
    else
    {
        // See if we can perform null-move pruning - this relies on not being in check or a king-and-pawn endgame.
        const std::size_t r { depth > 6 ? 4ULL : 3ULL };
        bool null_move_pruned { false };
        if (config::nmp && !in_check && !is_king_and_pawn_colour && depth >= r+1)
        {
            // No need to check legality here as we already know this move won't leave us in check.
            std::uint32_t unmake;
            make_move({ .check_legality = false }, gs, move::NULL_MOVE, unmake);
            const int score { -search_negamax_recursive(gs, stats, depth-r-1, -b, -b+1, -colour, move_buf) };
            unmake_move(gs, unmake);

            // Return early if this reduced search causes a beta-cutoff.
            if (score > b)
            {
                null_move_pruned = true;
                ret = score;
            }
        }

        // Continue with the rest of our search if we were unable to prune any nodes.
        if (!null_move_pruned)
        {
            // Otherwise, we need to continue the search by generating all nodes from here.
            const std::size_t moves { generate_pseudo_legal_moves(gs.bb, move_buf) };
            const std::span<std::uint32_t> move_list { move_buf.subspan(0, moves) };

            // Sort the moves favourably to increase the chance of early beta-cutoffs.
            sort_moves(move_list, draft, gs, pv_move, hash_move);

            for (std::size_t i =0; i < moves; i++)
            {
                const std::uint32_t move { move_list[i] };

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

                // Update the butterfly heuristic.
                if (config::hh) gs.hh.update_hh(move);

                // Do the actual search by recursing the algorithm. We vary the quality of the search based on whether this move is expected
                // to be any good. TODO: tweek the LMR kick-in after we add better move ordering (e.g. history-heuristic).
                int score;
                if (config::lmr && i >= 3 && depth > 3)
                {
                    // We run the recursive search at a lower depth if this move isn't near the top of our list after sorting. TODO: have a
                    // smarter adaptive LMR-reduction.
                    constexpr std::size_t lmr_reduction { 1 };
                    score = -search_negamax_recursive(gs, stats, depth-lmr_reduction-1, -b, -a, -colour, move_buf.subspan(moves));

                    // We rerun the search at full depth if the result of this reduced search fails high over beta (this move was better
                    // than we originally thought based on it's position in the move list).
                    if (score >= b)
                        score = -search_negamax_recursive(gs, stats, depth-1, -b, -a, -colour, move_buf.subspan(moves));
                }
                else
                {
                    // Otherwise, just run the recursive search without any reduction in depth.
                    score = -search_negamax_recursive(gs, stats, depth-1, -b, -a, -colour, move_buf.subspan(moves));
                }

                // Resets the game state to how it was before we made the move.
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
                {
                    // Update the history heuristic and store it as a killer move if it is quiet.
                    if (const auto type = move::deserialise_move_type(move); !move::move_type::is_capture(type) && !move::move_type::is_promotion(type))
                    {
                        gs.km.store_killer_move(draft, move);
                        if (config::hh) gs.hh.update_bf(move);
                    }

                    break;
                }
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

}

inline int search_negamax(game_state& gs, statistics& stats, std::size_t depth, int colour, std::span<std::uint32_t> move_buf) noexcept
{
    return details::search_negamax_recursive(gs, stats, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, move_buf);
}

inline int search_negamax_aspiration_window(game_state& gs, statistics& stats, std::size_t depth, int colour, std::span<std::uint32_t> move_buf) noexcept
{
    int a { -std::numeric_limits<int>::max() };
    int b {  std::numeric_limits<int>::max() };

    constexpr int initial_delta { 35 };
    int d = initial_delta;

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

        // Just do a regular non-aspiration-window search if we think we've found checkmate - not doing this can be very risky, and
        // can even end up in us recommending illegal moves!
        if (std::abs(score) >= evaluation::EVAL_CHECKMATE)
            return search_negamax(gs, stats, depth, colour, move_buf);

        // Otherwise check that it fits within the window, and adjust if necessary.
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