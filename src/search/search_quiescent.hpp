#pragma once

#include "search/statistics.hpp"

#include "evaluation/evaluate.hpp"
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

inline int mvv_lva_score(const bitboard& bb, std::uint64_t move)
{
    const auto attacker = move::make_decode_piece_idx(move);
    const auto victim   = bb.get_piece_type_colour(1ULL << move::make_decode_to_mb(move), !bb.is_black_to_play());

    const int victim_val   { std::abs(::evaluation::piece_mg_evaluation[victim]) };
    const int attacker_val { std::abs(::evaluation::piece_mg_evaluation[attacker]) };

    // Multiplying the victim score ensures it's the dominant term.
    return (10*victim_val) - attacker_val;
}

inline void sort_mvv_lva(const bitboard& bb, std::span<std::uint64_t> move_buf) noexcept
{
    std::array<std::pair<std::uint64_t, int>, MAX_MOVES_PER_POSITION> scored_moves;
    for (std::size_t i = 0; i < move_buf.size(); i++)
        scored_moves[i] = { move_buf[i], mvv_lva_score(bb, move_buf[i]) };

    std::sort(scored_moves.begin(), scored_moves.begin() + move_buf.size(), [](const auto& a, const auto& b) noexcept { return a.second > b.second; });

    for (std::size_t i = 0; i < move_buf.size(); i++)
        move_buf[i] = scored_moves[i].first;
}

}

inline int search_quiescence( game_state& gs, statistics& stats, std::size_t draft, int a, int b, int colour, std::span<std::uint64_t> move_buf) noexcept
{
    stats.qnodes++;
    stats.qdepth = std::max(stats.qdepth, draft);

    // Stand-pat evaluation (side to move perspective).
    const int stand_pat = colour*evaluation::evaluate(gs.bb);

    // Fail-hard beta cutoff.
    if (stand_pat >= b)
        return stand_pat;

    a = std::max(a, stand_pat);

    // Generate "noisy" moves (for now just captures).
    const std::size_t moves { generate_pseudo_legal_loud_moves(gs.bb, move_buf) };
    const std::span<std::uint64_t> move_list{ move_buf.subspan(0, moves) };

    // Sort quiescent moves based on MVV/LVA.
    details::sort_mvv_lva(gs.bb, move_list);

    for (const auto move : move_list)
    {
        if (gs.stop_search) [[unlikely]]
            return stand_pat;

        // Skip bad captures.
        if (config::see && evaluation::see_capture(gs.bb, move, colour == -1) < 0)
            continue;

        // Delta-prunning. TODO: turn off in late end-game.
        {
            constexpr int delta { 200 };
            const auto victim = gs.bb.get_piece_type_colour(1ULL << move::make_decode_to_mb(move), !gs.bb.is_black_to_play());
            const int victim_val { std::abs(::evaluation::piece_mg_evaluation[victim]) };
            if (victim_val + delta + stand_pat < a) [[unlikely]]
                continue;
        }

        std::uint64_t unmake;
        if (!make_move({ .check_legality = true }, gs, move, unmake)) [[unlikely]]
        {
            unmake_move(gs, unmake);
            continue;
        }

        int score = -search_quiescence(gs, stats, draft+1, -b, -a, -colour, move_buf.subspan(moves));
        unmake_move(gs, unmake);

        a = std::max(a, score);
        if (a >= b)
            break;
    }

    return a;
}

}