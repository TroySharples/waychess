#pragma once

// ####################################
// DECLARATION
// ####################################

#include "position/game_state.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "evaluation/evaluation.hpp"
#include "search/search_negamax.hpp"
#include "utility/logging.hpp"

#include <limits>
#include <sstream>
#include <vector>
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

recommendation recommend_move(game_state& gs, std::uint8_t max_depth, evaluation::evaluation eval);
recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, evaluation::evaluation eval);
recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, std::chrono::duration<double> max_time, evaluation::evaluation eval);

}

// ####################################
// IMPLEMENTATION
// ####################################

#include "search/transposition_table.hpp"

#include <future>

namespace search
{

namespace details { void log_search_info(game_state& gs, const statistics& stats, int eval); }

inline recommendation recommend_move(game_state& gs, std::uint8_t max_depth, evaluation::evaluation eval)
{
    // We time this whole thing for logging.
    const auto start = std::chrono::steady_clock::now();

    gs.stop_search = false;

    std::vector<std::uint32_t> move_buf(static_cast<std::size_t>(::details::pv_table::MAX_DEPTH*MAX_MOVES_PER_POSITION));
    std::span<std::uint32_t> move_span(move_buf);

    const bool is_black_to_play { gs.bb.is_black_to_play() };
    const std::uint8_t moves    { generate_pseudo_legal_moves(gs.bb, move_buf) };

    recommendation ret { .move = 0, .eval = is_black_to_play ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min() };

    // Set our root node and clear our transposition table.
    gs.root_ply = gs.bb.ply_counter;
    gs.age++;

    // Initialise our statistics.
    statistics stats;

    for (std::uint8_t i = 0; i < moves; i++)
    {
        std::uint32_t unmake;
        if (make_move({ .check_legality = true }, gs, move_buf[i], unmake)) [[likely]]
            if (const int score { search_negamax(gs, stats, max_depth-1, move_span.subspan(moves), eval) }; (is_black_to_play ? score < ret.eval : score > ret.eval) )
                ret = { .move = move_buf[i], .eval = score };
        unmake_move(gs, unmake);
    }

    gs.last_score = ret.eval;
    gs.pv.update(0, ret.move);

    const auto end = std::chrono::steady_clock::now();
    stats.duration = end - start;

    // Log our search info if we weren't stopped.
    if (!gs.stop_search)
        details::log_search_info(gs, stats, ret.eval);

    return ret;
}

namespace details
{

inline void log_search_info(game_state& gs, const statistics& stats, int eval)
{
    const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(stats.duration).count();
    const auto nps = static_cast<std::size_t>(static_cast<double>(stats.nodes) / std::chrono::duration<double>(stats.duration).count());

    std::ostringstream ss;
    ss << "depth "        << static_cast<int>(gs.pv.lengths[0])
       << " score cp "    << eval
       << " time "        << duration_ms
       << " nodes "       << stats.nodes
       << " nps "         << nps
       << " pv "          << move::to_algebraic_long(gs.pv.get_pv())
       << " tt-probes "   << stats.tt_probes
       << " tt-hits "     << stats.tt_hits
       << " tt-moves "    << stats.tt_moves
       << " tt-reorders " << stats.tt_reorders;
    log(ss.str(), log_level::informational);
}

}

inline recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, evaluation::evaluation eval)
{
    gs.stop_search = false;

    std::uint8_t depth { 1 };
    recommendation ret;

    // Only clear the PV table at the start of the ID search.
    gs.pv.clear();

    // Do the iterative deepening - we make sure to only update our recommendation if we weren't interrupted.
    while (depth <= max_depth)
    {
        const recommendation id = recommend_move(gs, depth++, eval);
        if (gs.stop_search)
            break;
        ret = id;
    }

    return ret;
}

inline recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, std::chrono::duration<double> max_time, evaluation::evaluation eval)
{
    auto f = std::async(static_cast<recommendation(*)(game_state&, std::uint8_t, evaluation::evaluation)>(&recommend_move_id), std::ref(gs), max_depth, eval);

    // Technically this is undefined multithreaded behaviour... Will improve this section of the code greatly in the future.
    std::this_thread::sleep_for(max_time);
    gs.stop_search = true;

    return f.get();
}

}