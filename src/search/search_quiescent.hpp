#pragma once

#include "pieces/pieces.hpp"
#include "search/statistics.hpp"

#include "evaluation/see.hpp"
#include "position/bitboard.hpp"
#include "position/move.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include "config.hpp"

namespace search
{

namespace details
{

inline int mvv_lva_score(const bitboard& bb, std::uint32_t move)
{
    const auto attacker = move::make_decode_piece_idx(move);
    const auto victim   = bb.get_piece_type_colour(1ULL << move::make_decode_to_mb(move), !bb.is_black_to_play());

    const int victim_val   { std::abs(::evaluation::piece_mg_evaluation[victim]) };
    const int attacker_val { std::abs(::evaluation::piece_mg_evaluation[attacker]) };

    // Multiplying the victim score ensures it's the dominant term.
    return (10*victim_val) - attacker_val;
}

constexpr int mvv_lva_score_min   { 10*::evaluation::piece_mg_evaluation[piece_idx::w_pawn]  - ::evaluation::piece_mg_evaluation[piece_idx::w_queen] };
constexpr int mvv_lva_score_max   { 10*::evaluation::piece_mg_evaluation[piece_idx::w_queen] - ::evaluation::piece_mg_evaluation[piece_idx::w_pawn]  };
constexpr int mvv_lva_score_range { mvv_lva_score_max - mvv_lva_score_min };

inline void score_movequiescent(std::int64_t& move, const game_state& gs) noexcept
{
    constexpr int32_t score_promotion { std::numeric_limits<int>::max()/2 };

    int64_t score;
    if (move & move::type::PROMOTION) [[unlikely]]
    {
        score = score_promotion;
    }
    else
    {
        score = mvv_lva_score(gs.bb, move);
    }

    // Set the score.
    move |= static_cast<std::int64_t>(score << 32);
}

inline void sort_moves_quiescent(std::span<std::int64_t> move_buf, const game_state& gs) noexcept
{
    // Score each move and fill-out the move-info.
    std::for_each(move_buf.begin(), move_buf.end(), [&gs] (std::int64_t& move) { score_movequiescent(move, gs); });

    // Sort the moves (high-to-low).
    std::sort(move_buf.rbegin(), move_buf.rend());
}

}

inline int search_quiescence(game_state& gs, statistics& stats, std::size_t draft, int a, int b, int colour, std::span<std::int64_t> move_buf) noexcept
{
    stats.qnodes++;
    stats.qdepth = std::max(stats.qdepth, draft);

    // Stand-pat evaluation (side to move perspective).
    const int stand_pat = colour*gs.evaluate();

    // Fail-hard beta cutoff.
    if (stand_pat >= b)
        return stand_pat;

    a = std::max(a, stand_pat);

    // Generate "noisy" moves (for now just captures).
    const std::size_t moves { generate_pseudo_legal_loud_moves(gs.bb, move_buf) };
    const auto move_list = move_buf.subspan(0, moves);

    // Sort quiescent moves.
    details::sort_moves_quiescent(move_list, gs);

    for (const auto make : move_list)
    {
        if (gs.stop_search) [[unlikely]]
            return stand_pat;

        // Possibly skip some non-promotion loud moves (i.e. non-promotion-captures) moves.
        if (!(make & move::type::PROMOTION))
        {
            // Skip bad captures.
            if (config::see && evaluation::see_capture(gs.bb, make, colour == -1) < 0)
                continue;

            // Delta-prunning. TODO: turn off in late end-game.
            {
                constexpr int delta { 200 };
                const auto victim = gs.bb.get_piece_type_colour(1ULL << move::make_decode_to_mb(make), !gs.bb.is_black_to_play());
                const int victim_val { std::abs(::evaluation::piece_mg_evaluation[victim]) };
                if (victim_val + delta + stand_pat < a) [[unlikely]]
                    continue;
            }
        }

        std::uint32_t unmake;
        if (!make_move({ .check_legality = true }, gs, make, unmake)) [[unlikely]]
        {
            unmake_move(gs, make, unmake);
            continue;
        }

        int score = -search_quiescence(gs, stats, draft+1, -b, -a, -colour, move_buf.subspan(moves));
        unmake_move(gs, make, unmake);

        a = std::max(a, score);
        if (a >= b)
            break;
    }

    return a;
}

}