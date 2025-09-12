#pragma once

#include "bitboard.hpp"
#include "details/pv_table.hpp"
#include "details/km_table.hpp"
#include "details/transposition_table.hpp"

// The main game state that is used in the search and evaluation. This includes the position itself (i.e. bitboard) as well
// as other incrementally updated fields (e.g. hash).
struct game_state
{
    game_state() = default;

    // Load up the game from a bitboard.
    void load(const bitboard& bb);

    // Clear all state - note that this doesn't affect the size of the transposition table.
    void reset();

    // The bitboard itself. We use a bitboard for obvious reasons.
    bitboard bb;

    // Zobrist hash of the current position.
    std::uint64_t hash;

    // The maximum theoretical game limit - this is a good-chunk of memory but we only have one of them so it's okay.
    static constexpr std::size_t MAX_GAME_LENGTH { 11798 };

    // A boolean indicating when we must stop searching as soon as possible. All search algorithms must respect this.
    bool stop_search;

    // History of hashes (LSB 32b) of previous positions indexed by the ply, as well as the ply of the last non-reversible
    // move.
    std::array<std::uint32_t, MAX_GAME_LENGTH> position_history;

    // The ply of our root node in our search.
    std::uint16_t root_ply;

    // The main transposition table.
    details::transposition_table tt;

    // The age of the current game-state. This is for things like invalidating hash lookups for when we need to consider
    // new positions.
    std::uint8_t age;

    // PV table for collection.
    details::pv_table pv;

    // Killer move table for collection.
    details::km_table km;

    // Prints the PV, assuming this game state is ply-deep into the search (0 if we aren't searching). The additional work
    // this does is that it ensures only legal PVs are printed.
    std::span<const std::uint32_t> get_pv(std::uint8_t ply = 0) const noexcept;

    // The last ID run score.
    int eval;

    // TODO: There are a lot more things we should add to do with the evaluation (e.g. pawn-structure tables).

    // Determines whether the current position a draw by either the 50-move rule, or the three-fold-repetition rule - should
    // be called early on in search evaluation.
    bool is_repetition_draw() const noexcept;
};

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// IMPLEMENTATION
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

inline bool game_state::is_repetition_draw() const noexcept
{
    // These kinds of draws are impossible if we haven't even made enough non-reversible moves.
    if (bb.ply_50m < 6) [[likely]]
        return false;

    // It's an immediate 50-move-rule draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return true;

    // Otherwise, we need to start looking back through our position history and count the occurrences of our current hash value
    // since the last non-reversible move. Note we use a signed integer type here to avoid possible underflows.
    const int last_reversible_ply { bb.ply_counter - bb.ply_50m };

    std::size_t repetitions {};
    for (int ply { bb.ply_counter-2 }; ply >= last_reversible_ply; ply -= 2)
        if (position_history[ply] == static_cast<std::uint32_t>(hash)) [[unlikely]]
            repetitions++;

    // If we've seen this position at least twice before, then this is at least our third repetition and it is a draw.
    return repetitions >= 2;
}