#pragma once

#include "evaluation/evaluate.hpp"
#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"
#include "position/move.hpp"
#include "search.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

namespace search
{

namespace details
{

inline int mvv_lva_score(const bitboard& bb, std::uint32_t move)
{
    const auto attacker = static_cast<std::uint8_t>(0x07 & move::deserialise_piece_idx(move));
    const auto victim   = static_cast<std::uint8_t>(0x07 & move::get_victim_piece_idx(move, bb));

    const int victim_val   { ::evaluation::piece_mg_evaluation[static_cast<std::uint8_t>(victim)] };
    const int attacker_val { ::evaluation::piece_mg_evaluation[static_cast<std::uint8_t>(attacker)] };

    // Multiplying the victim score ensures it's the dominant term.
    return (10*victim_val) - attacker_val;
}

inline void sort_mvv_lva(const bitboard& bb, std::span<std::uint32_t> move_buf) noexcept
{
    std::sort(move_buf.begin(), move_buf.end(), [&bb](std::uint32_t a, std::uint32_t b) { return mvv_lva_score(bb, a) > mvv_lva_score(bb, b); });
}

}

inline int search_quiescence( game_state& gs, statistics& stats, int a, int b, int colour, std::span<std::uint32_t> move_buf) noexcept
{
    stats.nodes++;

    // Stand-pat evaluation (side to move perspective).
    const int stand_pat = colour*evaluation::evaluate(gs.bb);

    // Fail-hard beta cutoff.
    if (stand_pat >= b)
        return stand_pat;

    a = std::max(a, stand_pat);

    // Generate "noisy" moves (for now just captures).
    const std::uint8_t moves = generate_pseudo_legal_loud_moves(gs.bb, move_buf);
    const std::span<std::uint32_t> move_list{ move_buf.subspan(0, moves) };

    // Sort quiescent moves based on MVV/LVA.
    details::sort_mvv_lva(gs.bb, move_list);

    for (const auto move : move_list)
    {
        if (gs.stop_search) [[unlikely]]
            return stand_pat;

        // Delta-prunning.
        {
            constexpr int delta { 200 };
            const auto victim = static_cast<std::uint8_t>(0x07 & move::get_victim_piece_idx(move, gs.bb));
            const int victim_val { ::evaluation::piece_mg_evaluation[victim] };
            if (victim_val + delta + stand_pat < a) [[unlikely]]
                continue;
        }

        std::uint32_t unmake;
        if (!make_move({ .check_legality = true }, gs, move, unmake)) [[unlikely]]
        {
            unmake_move(gs, unmake);
            continue;
        }

        int score = -search_quiescence(gs, stats, -b, -a, -colour, move_buf.subspan(moves));
        unmake_move(gs, unmake);

        a = std::max(a, score);
        if (a >= b)
            break;
    }

    return a;
}

}