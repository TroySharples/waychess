#pragma once

#include "pv_table.hpp"

namespace details
{

struct km_table
{
    static constexpr std::size_t KM_MOVES { 2 };
    std::array<std::array<std::uint32_t, KM_MOVES>, pv_table::MAX_DEPTH> table;

    bool is_killer_move(std::size_t ply, std::uint32_t move) const noexcept;
    void store_killer_move(std::size_t ply, std::uint32_t move) noexcept;

    // Wipe all state from the table - this should only be used when starting a new game.
    void reset() noexcept;
};

inline bool km_table::is_killer_move(std::size_t ply, std::uint32_t move) const noexcept
{
    return table[ply][0] == move || table[ply][1] == move;
}

inline void km_table::store_killer_move(std::size_t ply, std::uint32_t move) noexcept
{
    // Shift the old killer moves down.
    if (move != table[ply][0])
    {
        std::swap(table[ply][0], table[ply][1]);
        table[ply][0] = move;
    }
}

inline void km_table::reset() noexcept
{
    table.fill({});
}

}