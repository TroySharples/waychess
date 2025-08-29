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
    std::uint16_t last_non_reversible_ply {};

    // The age of the current game-state. This is for things like invalidating hash lookups for when we need to consider
    // new positions.
    std::uint8_t age;

    // TODO: There are a lot more things we should add to do with the evaluation (e.g. pawn-structure tables).
};