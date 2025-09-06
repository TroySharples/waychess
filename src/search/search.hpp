#pragma once

// ####################################
// DECLARATION
// ####################################

#include "position/game_state.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "search/search_negamax.hpp"
#include "search/statistics.hpp"
#include "utility/logging.hpp"

#include <limits>
#include <sstream>
#include <chrono>

namespace search
{

struct recommendation
{
    std::uint32_t move;
    int eval;

    friend constexpr bool operator<(const recommendation& a, const recommendation& b) noexcept { return a.eval < b.eval; };
    friend constexpr bool operator>(const recommendation& a, const recommendation& b) noexcept { return a.eval > b.eval; };
};

recommendation recommend_move(game_state& gs, statistics& stats, std::uint8_t max_depth);
recommendation recommend_move_id(game_state& gs, statistics& stats, std::uint8_t max_depth);
recommendation recommend_move_id(game_state& gs, statistics& stats, std::uint8_t max_depth, std::chrono::duration<double> max_time);

recommendation recommend_move(game_state& gs, std::uint8_t max_depth);
recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth);
recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, std::chrono::duration<double> max_time);

}

// ####################################
// IMPLEMENTATION
// ####################################

#include <future>

namespace search
{

inline recommendation recommend_move(game_state& gs, statistics& stats, std::uint8_t max_depth)
{
    // We time this whole thing for logging.
    const auto start = std::chrono::steady_clock::now();

    gs.stop_search = false;

    std::array<std::uint32_t, static_cast<std::size_t>(::details::pv_table::MAX_DEPTH*MAX_MOVES_PER_POSITION)> move_buf;
    std::span<std::uint32_t> move_span(move_buf);

    // Set our root node, update our transposition table age, and propagate our root PV to our upper ply.
    gs.root_ply = gs.bb.ply_counter;
    gs.age++;
    gs.pv.init_from_root();

    // Initialise our local statistics for this ID run.
    statistics stats_local = {};
    stats_local.depth = max_depth;

    const bool is_black_to_play { gs.bb.is_black_to_play() };
    const std::uint8_t moves    { generate_pseudo_legal_moves(gs.bb, move_span) };
    const std::span<std::uint32_t> move_list { move_span.subspan(0, moves) };

    // Sort our move by PV to at least we get to use the upper-ply information before it gets corrupted - there's no harm in
    // doing this for our initial depth-1 search.
    details::sort_moves(move_list, gs.pv.table[0][0]);

    recommendation ret { .move = 0, .eval = is_black_to_play ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min() };
    for (std::uint8_t i = 0; i < moves; i++)
    {
        std::uint32_t unmake;
        if (const std::uint32_t move { move_list[i] }; make_move({ .check_legality = true }, gs, move, unmake)) [[likely]]
            if (const int score { max_depth == 1 ? search_negamax(gs, stats_local, max_depth-1, move_span.subspan(moves)) : search_negamax_aspiration_window(gs, stats_local, max_depth-1, move_span.subspan(moves)) }; (is_black_to_play ? score <= ret.eval : score >= ret.eval) )
                ret = { .move = move, .eval = score };
        unmake_move(gs, unmake);
    }

    gs.last_score = ret.eval;
    gs.pv.update_variation(0, ret.move);

    const auto end = std::chrono::steady_clock::now();
    stats_local.time = end - start;

    stats_local.pv = gs.get_pv(0);

    // Log and update our search info if we weren't stopped.
    if (!gs.stop_search)
    {
        stats_local.log_search_info();
        stats.id_update(stats_local);
    }

    return ret;
}
inline recommendation recommend_move_id(game_state& gs, statistics& stats, std::uint8_t max_depth)
{
    gs.stop_search = false;

    std::uint8_t depth { 1 };
    recommendation ret;

    // Do the iterative deepening - we make sure to only update our recommendation if we weren't interrupted.
    while (depth <= max_depth)
    {
        const recommendation id = recommend_move(gs, stats, depth++);
        if (gs.stop_search)
            break;
        ret = id;

        // If we've found checkmate we return immediately.
        if (std::abs(id.eval) == std::numeric_limits<int>::max())
            break;
    }

    return ret;
}

inline recommendation recommend_move_id(game_state& gs, statistics& stats, std::uint8_t max_depth, std::chrono::duration<double> max_time)
{
    auto f = std::async(static_cast<recommendation(*)(game_state&, statistics&, std::uint8_t)>(&recommend_move_id), std::ref(gs), std::ref(stats), max_depth);

    // Technically this is undefined multithreaded behaviour... Will improve this section of the code greatly in the future.
    std::this_thread::sleep_for(max_time);
    gs.stop_search = true;

    return f.get();
}

inline recommendation recommend_move(game_state& gs, std::uint8_t max_depth)
{
    statistics stats;
    return recommend_move(gs, stats, max_depth);
}

inline recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth)
{
    statistics stats;
    return recommend_move_id(gs, stats, max_depth);
}

inline recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, std::chrono::duration<double> max_time)
{
    statistics stats;
    return recommend_move_id(gs, stats, max_depth, max_time);
}


}