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

class pv_table
{
public:
    static constexpr std::size_t MAX_DEPTH { 64 };

    // Only clears the length storage.
    void clear() noexcept;

    // Called when we find a new best move at a given ply (distance from root).
    void update(std::size_t ply, std::uint32_t move);

    // Retrieve root PV after a search.
    std::span<const std::uint32_t> get_pv() const noexcept;

private:
    std::array<std::array<std::uint32_t, MAX_DEPTH>, MAX_DEPTH> _pv_table;
    std::array<std::size_t, MAX_DEPTH> _lengths {};
};

// ####################################
// IMPLEMENTATION
// ####################################

inline void pv_table::clear() noexcept
{
    _lengths.fill(0);
}

inline void pv_table::update(std::size_t ply, std::uint32_t move)
{
    _pv_table[ply][0] = move;

    // Copy the child's PV into this one.
    if (_lengths[ply + 1] > 0)
    {
        std::copy_n(_pv_table[ply + 1].begin(), _lengths[ply + 1], _pv_table[ply].begin() + 1);
        _lengths[ply] = _lengths[ply + 1] + 1;
    }
    else
    {
        _lengths[ply] = 1;
    }
}

inline std::span<const std::uint32_t> pv_table::get_pv() const noexcept
{
    return { _pv_table[0].data(), _lengths[0] };
}

}