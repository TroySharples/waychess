#pragma once

#include <array>
#include <cstdint>
#include <algorithm>

namespace details
{

// ####################################
// DECLARATION
// ####################################

struct pv_table
{
    static constexpr std::size_t MAX_DEPTH { 64 };

    // Sets our current best move, and propagates childs variation into this ply. This should be set after finding a new best
    // move by searching.
    void update_variation(std::size_t ply, std::uint64_t move) noexcept;

    // Fill out the upper-ply PVs assuming that we search the mainline PV first.
    void init_from_root() noexcept;

    // Wipe all state from the table - this should only be used when starting a new game.
    void reset() noexcept;

    std::array<std::array<std::uint64_t, MAX_DEPTH>, MAX_DEPTH> table;
};

// ####################################
// IMPLEMENTATION
// ####################################

inline void pv_table::update_variation(std::size_t ply, std::uint64_t move) noexcept
{
    table[ply][0] = move;
    std::copy_n(table[ply + 1].begin(), MAX_DEPTH-1, table[ply].begin() + 1);
}

inline void pv_table::init_from_root() noexcept
{
    for (std::size_t i = 1; i < table.size(); i++)
    {
        std::copy_n(table[0].begin() + i, MAX_DEPTH-i, table[i].begin());
    }
}

inline void pv_table::reset() noexcept
{
    table.fill({});
}

}