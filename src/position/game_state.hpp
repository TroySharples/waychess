#pragma once

#include "bitboard.hpp"

// The main game state that is used in the search and evaluation. This includes the position itself (i.e. bitboard) as well
// as other incrementally updated fields (e.g. hash).
struct game_state
{
    game_state() = default;
    game_state(const bitboard& bb);

    game_state& operator=(const bitboard& bb);

    // The bitboard itself. We use a bitboard for obvious reasons.
    bitboard bb;

    // Zobrist hash of the current position.
    std::uint64_t hash;

    // We only support game lengths up to 1024 ply (longest ever game between two engines is Stockfish vs Berserk at 467
    // moves = 934 ply). This is well below the theoretical limit of 11798 ply, but I like my memory...
    static constexpr std::uint16_t MAX_GAME_LENGTH { 1024 };

    // History of hashes (LSB 32b) of previous positions indexed by the ply, as well as the ply of the last non-reversible
    // move.
    std::array<std::uint32_t, MAX_GAME_LENGTH> position_history;

    // The ply of some key move positions in the tree.
    std::uint16_t root_ply {};

    // The age of the current game-state. This is for things like invalidating hash lookups for when we need to consider
    // new positions.
    std::uint8_t age;

    // TODO: There are a lot more things we should add to do with the evaluation (e.g. pawn-structure tables).

    // Determines whether the current position a draw by either the 50-move rule, or the three-fold-repetition rule - should
    // be called early on in search evaluation.
    bool is_repetition_draw() const noexcept;
};

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// IMPLEMENTATION
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include <algorithm>

inline bool game_state::is_repetition_draw() const noexcept
{
    // These kinds of draws are impossible if we haven't even made enough non-reversible moves.
    if (bb.ply_50m < 4) [[unlikely]]
        return false;

    // It's an immediate 50-move-rule draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return true;

    // Otherwise, we need to start looking back through our position history and count the occurrences of our current hash value
    // since the last non-reversible move. Note we use a signed integer type here to avoid possible underflows.
    const int last_reversible_ply { bb.ply_counter - bb.ply_50m };

    std::size_t repetitions {};
    for (int ply { bb.ply_counter }; ply >= last_reversible_ply; ply -= 2)
        if (position_history[ply] == hash) [[unlikely]]
            repetitions++;

    // If this is at least our third repetition then it is a draw.
    return repetitions >= 3;
}