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
#include <mutex>
#include <stdexcept>

namespace search
{

struct recommendation
{
    std::uint32_t move;
    int eval;

    friend constexpr bool operator<(const recommendation& a, const recommendation& b) noexcept { return a.eval < b.eval; };
    friend constexpr bool operator>(const recommendation& a, const recommendation& b) noexcept { return a.eval > b.eval; };
};

recommendation recommend_move(game_state& gs, statistics& stats, std::size_t max_depth = 64, std::chrono::duration<double> max_time = std::chrono::hours(2));
recommendation recommend_move(game_state& gs, std::size_t max_depth = 64, std::chrono::duration<double> max_time = std::chrono::hours(2)) { statistics stats; return recommend_move(gs, stats, max_depth, max_time); }

}

// ####################################
// IMPLEMENTATION
// ####################################

#include <future>

namespace search
{

namespace details
{

// Is used by the iterative-deepening recommend-move call.
inline recommendation recommend_move_impl(game_state& gs, statistics& stats, std::size_t depth)
{
    std::array<std::uint32_t, static_cast<std::size_t>(::details::pv_table::MAX_DEPTH*MAX_MOVES_PER_POSITION)> move_buf;
    std::span<std::uint32_t> move_span(move_buf);

    // Set our root node, update our transposition table age, and propagate our root PV to our upper ply.
    gs.root_ply = gs.bb.ply_counter;
    gs.age++;
    gs.pv.init_from_root();

    // Initialise our local statistics for this ID run.
    statistics stats_local = {};
    stats_local.depth = depth;

    // Negamax returns a relative score for the side to play, so we have to multiply it by the colour.
    const int colour { gs.bb.is_black_to_play() ? -1 : 1 };

    const auto start = std::chrono::steady_clock::now();
    gs.eval = colour*search_negamax_aspiration_window(gs, stats_local, depth, colour, move_span);
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

inline recommendation recommend_move_id_impl(game_state& gs, statistics& stats, std::size_t depth)
{
    gs.stop_search = false;

    // Do the iterative deepening - we make sure to only update our recommendation if we weren't interrupted.
    recommendation ret;
    for (std::size_t i = 1; i <= depth; i++)
    {
        const recommendation id = details::recommend_move_impl(gs, stats, i);
        if (gs.stop_search)
            break;
        ret = id;

        // If we've found checkmate we return immediately.
        if (std::abs(id.eval) >= evaluation::EVAL_CHECKMATE)
            break;
    }

    gs.stop_search = true;
    return ret;
}

class search_impl
{
public:
    recommendation recommend_move(game_state& gs, statistics& stats, std::size_t max_depth, std::chrono::duration<double> max_time)
    {
        // Make sure our thread is not joined.
        if (_t.joinable())
            throw std::runtime_error("Cannot start search when our search thread has not been joined");

        // Reset our recommendation and kick-off our search thread.
        _rec.reset();
        _t = std::thread(&search_impl::run, this, std::ref(gs), std::ref(stats), max_depth);

        // Set a condition variable that waits for the search to stop, or an amount of time to elapse.
        std::unique_lock<std::mutex> lk(_m);
        _c.wait_for(lk, max_time, [this] () { return _rec.has_value(); });

        // Stop the search if we haven't already and join the running thread.
        gs.stop_search = true;
        _t.join();

        return *_rec;
    }

private:
    std::thread _t;
    std::condition_variable _c;
    std::optional<recommendation> _rec;
    std::mutex _m;

    void run(game_state& gs, statistics& stats, std::size_t max_depth)
    {
        recommendation rec = details::recommend_move_id_impl(gs, stats, max_depth);
        {
            std::lock_guard<std::mutex> lk(_m);
            _rec = rec;
        }
        _c.notify_one();
    }
};

}

inline recommendation recommend_move(game_state& gs, statistics& stats, std::size_t max_depth, std::chrono::duration<double> max_time)
{
    details::search_impl s;
    return s.recommend_move(gs, stats, max_depth, max_time);
}


}