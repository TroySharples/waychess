#pragma once

// ####################################
// DECLARATION
// ####################################

#include "evaluation/evaluation.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <vector>
#include <chrono>

namespace search
{

// A general search signature. Functions of this type take a terminal heuristic, and return the
// searched evaluation. The behaviour of the search may be changed with the args pointer (e.g.
// max-depth). This must return an absolute centipawn evaluation (i.e. not relative to the
// playing side).
using search = int (*)(const bitboard& bb, std::uint8_t max_depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval);

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

recommendation recommend_move(const bitboard& bb, search s, std::uint8_t max_depth, evaluation::evaluation eval, const void* args_eval = nullptr);

recommendation recommend_move_id(const bitboard& bb, search s, std::chrono::duration<double> time, evaluation::evaluation eval, const void* args_eval = nullptr);

}

// ####################################
// IMPLEMENTATION
// ####################################

#include <future>

namespace search
{

inline recommendation recommend_move(const bitboard& bb, search s, std::uint8_t max_depth, evaluation::evaluation eval, const void* args_eval)
{
    stop_search = false;

    std::vector<std::uint32_t> move_buf(static_cast<std::size_t>(max_depth)*MAX_MOVES_PER_POSITION);
    std::span<std::uint32_t> move_span(move_buf);

    const bool is_black_to_play { bb.is_black_to_play() };
    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    recommendation ret { .move = 0, .eval = is_black_to_play ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min() };

    for (std::uint8_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        if (const int score { s(next_position, max_depth-1, move_span.subspan(moves), eval, args_eval) }; (is_black_to_play ? score < ret.eval : score > ret.eval) )
            ret = { .move = move_buf[i], .eval = score };
    }

    return ret;
}

namespace details
{

inline recommendation recommend_move_id_future(const bitboard& bb, search s, evaluation::evaluation eval, const void* args_eval)
{
    stop_search = false;

    std::uint8_t depth { 1 };
    recommendation ret;

    // Do the iterative deepening - we make sure to only update our recommendation if we weren't interrupted.
    while (true)
    {
        const recommendation id = recommend_move(bb, s, depth++, eval, args_eval);
        if (stop_search)
            break;
        ret = id;
    }

    return ret;
}

}

inline recommendation recommend_move_id(const bitboard& bb, search s, std::chrono::duration<double> time, evaluation::evaluation eval, const void* args_eval)
{
    auto f = std::async(&details::recommend_move_id_future, bb, s, eval, args_eval);

    // Technically this is undefined multithreaded behaviour... Will improve this section of the code greatly in the future.
    std::this_thread::sleep_for(time);
    stop_search = true;

    return f.get();
}

}