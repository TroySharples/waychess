#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <algorithm>

namespace details
{

// ####################################
// DECLARATION
// ####################################

struct pv_table
{
    static constexpr std::uint8_t MAX_DEPTH { 64 };

    // Only clears the length storage.
    void clear() noexcept;

    // Called when we find a new best move at a given ply (distance from root).
    void update(std::uint8_t ply, std::uint32_t move);

    // Retrieve root PV after a search.
    std::span<const std::uint32_t> get_pv() const noexcept;

    std::array<std::array<std::uint32_t, MAX_DEPTH>, MAX_DEPTH> table;
    std::array<std::uint8_t, MAX_DEPTH> lengths {};
};

// ####################################
// IMPLEMENTATION
// ####################################

inline void pv_table::clear() noexcept
{
    lengths.fill(0);
}

inline void pv_table::update(std::uint8_t ply, std::uint32_t move)
{
    table[ply][0] = move;

    // Copy the child's PV into this one.
    if (lengths[ply + 1] > 0)
    {
        std::copy_n(table[ply + 1].begin(), lengths[ply + 1], table[ply].begin() + 1);
        lengths[ply] = lengths[ply + 1] + 1;
    }
    else
    {
        lengths[ply] = 1;
    }
}

inline std::span<const std::uint32_t> pv_table::get_pv() const noexcept
{
    return { table[0].data(), lengths[0] };
}

}