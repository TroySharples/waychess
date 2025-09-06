#pragma once

// ####################################
// DECLARATION
// ####################################

#include "position/game_state.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
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

recommendation recommend_move(game_state& gs, std::uint8_t max_depth);
recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth);
recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, std::chrono::duration<double> max_time);

}

// ####################################
// IMPLEMENTATION
// ####################################

#include "search/transposition_table.hpp"

#include <future>

namespace search
{

namespace details { void log_search_info(std::uint8_t depth, const game_state& gs, const statistics& stats, int eval); }

inline recommendation recommend_move(game_state& gs, std::uint8_t max_depth)
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

    // Initialise our statistics.
    statistics stats;

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
            if (const int score { max_depth == 1 ? search_negamax(gs, stats, max_depth-1, move_span.subspan(moves)) : search_negamax_aspiration_window(gs, stats, max_depth-1, move_span.subspan(moves)) }; (is_black_to_play ? score <= ret.eval : score >= ret.eval) )
                ret = { .move = move, .eval = score };
        unmake_move(gs, unmake);
    }

    gs.last_score = ret.eval;
    gs.pv.update_variation(0, ret.move);

    const auto end = std::chrono::steady_clock::now();
    stats.duration = end - start;

    // Log our search info if we weren't stopped.
    if (!gs.stop_search)
        details::log_search_info(max_depth, gs, stats, ret.eval);

    return ret;
}

namespace details
{

inline void log_search_info(std::uint8_t depth, const game_state& gs, const statistics& stats, int eval)
{
    const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(stats.duration).count();
    const auto nps = static_cast<std::size_t>(static_cast<double>(stats.nodes) / std::chrono::duration<double>(stats.duration).count());

    std::ostringstream ss;
    ss << "depth "      << static_cast<int>(depth)
       << " score cp "  << eval
       << " time "      << duration_ms
       << " nodes "     << stats.nodes
       << " nps "       << nps
       << " pv "        << move::to_algebraic_long(gs.get_pv());
    log(ss.str(), log_level::informational);
}

}

inline recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth)
{
    gs.stop_search = false;

    std::uint8_t depth { 1 };
    recommendation ret;

    // Do the iterative deepening - we make sure to only update our recommendation if we weren't interrupted.
    while (depth <= max_depth)
    {
        const recommendation id = recommend_move(gs, depth++);
        if (gs.stop_search)
            break;
        ret = id;

        // If we've found checkmate we return immediately.
        if (std::abs(id.eval) == std::numeric_limits<int>::max())
            break;
    }

    return ret;
}

inline recommendation recommend_move_id(game_state& gs, std::uint8_t max_depth, std::chrono::duration<double> max_time)
{
    auto f = std::async(static_cast<recommendation(*)(game_state&, std::uint8_t)>(&recommend_move_id), std::ref(gs), max_depth);

    // Technically this is undefined multithreaded behaviour... Will improve this section of the code greatly in the future.
    std::this_thread::sleep_for(max_time);
    gs.stop_search = true;

    return f.get();
}

}