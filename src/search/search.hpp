#pragma once

// ####################################
// DECLARATION
// ####################################

#include "evaluation/evaluate.hpp"
#include "position/game_state.hpp"
#include "position/generate_moves.hpp"
#include "search/search_negamax.hpp"
#include "search/statistics.hpp"

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

    // Negamax returns a relative score for the side to play, so we have to multiply it by the colour.
    const int colour { gs.bb.is_black_to_play() ? -1 : 1 };

    const auto start = std::chrono::steady_clock::now();
    gs.eval = colour*search_negamax_aspiration_window(gs, stats_local, max_depth, colour, move_span);
    const auto end = std::chrono::steady_clock::now();

    stats_local.time = end - start;
    stats_local.pv = gs.get_pv(0);

    // Log and update our search info if we weren't stopped.
    if (!gs.stop_search)
    {
        stats_local.log_search_info();
        stats.id_update(stats_local);
    }

    return { .move=gs.pv.table[0][0], .eval=gs.eval };
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
        if (std::abs(id.eval) >= evaluation::EVAL_CHECKMATE)
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