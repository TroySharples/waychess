#pragma once

#include "evaluation/evaluate_raw_material.hpp"
#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"
#include "position/move.hpp"
#include "search.hpp"

#include "evaluation/evaluation.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

namespace search
{

namespace details
{

inline int mvv_lva_score(const bitboard& bb, std::uint32_t move)
{
    const bool is_black_to_play { bb.is_black_to_play() };

    const piece_idx attacker { move::deserialise_piece_idx(move) };

    // Working out the victim is a bit more involved...
    const piece_idx victim { [&]()
    {
        const std::uint64_t to_bb { 1ULL << move::deserialise_to_mb(move) };

        for (std::uint8_t capture_idx = (is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn); capture_idx <= (is_black_to_play ? piece_idx::w_queen : piece_idx::b_queen); capture_idx++)
            if (const std::uint64_t capture_bitboard = bb.boards[capture_idx]; capture_bitboard & to_bb)
                return static_cast<piece_idx>(capture_idx & 0x07);

        // If we haven't hit already, it must be an en-passent capture.
        return w_pawn;
    }() };

    const int victim_val   = ::evaluation::details::material_cp[static_cast<std::uint8_t>(victim)];
    const int attacker_val = ::evaluation::details::material_cp[static_cast<std::uint8_t>(attacker)];

    // MVV-LVA = Victim value * big factor - Attacker value
    // Multiplying victim_val ensures it's the dominant term.
    return (10*victim_val) - attacker_val;
}

inline void sort_mvv_lva(const bitboard& bb, std::span<std::uint32_t> move_buf) noexcept
{
    std::sort(move_buf.begin(), move_buf.end(),
              [&bb](std::uint32_t a, std::uint32_t b)
              {
                  return mvv_lva_score(bb, a) > mvv_lva_score(bb, b);
              });
}

}

inline int search_quiescence( game_state& gs, statistics& stats, int a, int b, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf) noexcept
{
    stats.nodes++;

    // Stand-pat evaluation (side to move perspective).
    const int stand_pat = colour*eval(gs.bb);

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
        if (stop_search) [[unlikely]]
            return stand_pat;

        std::uint32_t unmake;
        if (!make_move({ .check_legality = true }, gs, move, unmake))
        {
            unmake_move(gs, unmake);
            continue;
        }

        int score = -search_quiescence(gs, stats, -b, -a, -colour, eval, move_buf.subspan(moves));
        unmake_move(gs, unmake);

        a = std::max(a, score);
        if (a >= b)
            break;
    }

    return a;
}

}