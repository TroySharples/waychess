#pragma once

#include "pv_table.hpp"

namespace details
{

struct km_table
{
    static constexpr std::size_t KM_MOVES { 2 };
    std::array<std::array<std::uint32_t, KM_MOVES>, pv_table::MAX_DEPTH> km_table;

    bool is_killer_move(std::size_t ply, std::uint32_t move) const noexcept;
    void store_killer_move(std::size_t ply, std::uint32_t move) noexcept;
};

inline bool km_table::is_killer_move(std::size_t ply, std::uint32_t move) const noexcept
{
    return km_table[ply][0] == move || km_table[ply][1] == move;
}

inline void km_table::store_killer_move(std::size_t ply, std::uint32_t move) noexcept
{
    // Shift the old killer moves down.
    if (move != km_table[ply][0])
    {
        km_table[ply][1] = km_table[ply][0];
        km_table[ply][0] = move;
    }
}

}