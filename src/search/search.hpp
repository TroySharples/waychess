#pragma once

// ####################################
// DECLARATION
// ####################################

#include "position/game_state.hpp"
#include "evaluation/evaluation.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <vector>
#include <chrono>

namespace search
{

// A general search signature. Functions of this type take a terminal heuristic, and return the
// searched evaluation. This must return an absolute centipawn evaluation (i.e. not relative to the
// playing side).
using search = int (*)(game_state& gs, std::uint8_t max_depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval);

// A global boolean indicating when we must stop searching as soon as possible. All algorithms must
// respect this.
extern bool stop_search;

struct recommendation
{
    std::uint32_t move;
    int eval;

    friend constexpr bool operator<(const recommendation& a, const recommendation& b) noexcept { return a.eval < b.eval; };
    friend constexpr bool operator>(const recommendation& a, const recommendation& b) noexcept { return a.eval > b.eval; };
};

recommendation recommend_move(game_state& gs, search s, std::uint8_t max_depth, evaluation::evaluation eval);

recommendation recommend_move_id(game_state& gs, search s, std::chrono::duration<double> time, evaluation::evaluation eval);

}

// ####################################
// IMPLEMENTATION
// ####################################

#include "search/transposition_table.hpp"

#include <future>

namespace search
{

inline recommendation recommend_move(game_state& gs, search s, std::uint8_t max_depth, evaluation::evaluation eval)
{
    stop_search = false;

    std::vector<std::uint32_t> move_buf(static_cast<std::size_t>(max_depth)*MAX_MOVES_PER_POSITION);
    std::span<std::uint32_t> move_span(move_buf);

    const bool is_black_to_play { gs.bb.is_black_to_play() };
    const std::uint8_t moves    { generate_pseudo_legal_moves(gs.bb, move_buf) };

    recommendation ret { .move = 0, .eval = is_black_to_play ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min() };

    // Set our root node and clear our transposition table.
    gs.root_ply = gs.bb.ply_counter;
    gs.age++;

    for (std::uint8_t i = 0; i < moves; i++)
    {
        std::uint32_t unmake;
        if (make_move({ .check_legality = true }, gs, move_buf[i], unmake)) [[likely]]
            if (const int score { s(gs, max_depth-1, move_span.subspan(moves), eval) }; (is_black_to_play ? score < ret.eval : score > ret.eval) )
                ret = { .move = move_buf[i], .eval = score };
        unmake_move(gs, unmake);
    }

    gs.pv.update(0, ret.move);

    return ret;
}

namespace details
{

inline recommendation recommend_move_id_future(game_state& gs, search s, evaluation::evaluation eval)
{
    stop_search = false;

    std::uint8_t depth { 1 };
    recommendation ret;

    // Only clear the PV table at the start of the ID search.
    gs.pv.clear();

    // Do the iterative deepening - we make sure to only update our recommendation if we weren't interrupted.
    while (true)
    {
        const recommendation id = recommend_move(gs, s, depth++, eval);
        if (stop_search)
            break;
        ret = id;
    }

    return ret;
}

}

inline recommendation recommend_move_id(game_state& gs, search s, std::chrono::duration<double> time, evaluation::evaluation eval)
{
    auto f = std::async(&details::recommend_move_id_future, std::ref(gs), s, eval);

    // Technically this is undefined multithreaded behaviour... Will improve this section of the code greatly in the future.
    std::this_thread::sleep_for(time);
    stop_search = true;

    return f.get();
}

}