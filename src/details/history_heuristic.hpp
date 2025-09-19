#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace details
{

class history_heuristic
{
public:
    void reset() noexcept;

    // Higher bonuses are better - ambition bonuses should usually be between about -10 and 10;
    void update_hh(std::uint64_t move) noexcept { _score_hh[idx(move)]++; }
    void update_bf(std::uint64_t move) noexcept { _score_bf[idx(move)]++; }

    // Just get the score of the move.
    int get_bonus(std::uint64_t move) const noexcept;

    static constexpr int MAX_HISTORY_BONUS = 536870912;

private:
    // Uses the first 12-bits (to + from squares in move) as the index.
    static constexpr std::uint64_t IDX_MASK { 0x000001ff };
    static constexpr std::size_t idx(std::uint64_t move) noexcept{ return (move >> 6) & IDX_MASK; }

    std::array<std::size_t, 512> _score_hh;
    std::array<std::size_t, 512> _score_bf;
};

inline void history_heuristic::reset() noexcept
{
    _score_bf.fill(0);
    _score_hh.fill(0);
}

inline int history_heuristic::get_bonus(std::uint64_t move) const noexcept
{
    return static_cast<int>(static_cast<std::size_t>(MAX_HISTORY_BONUS)*(_score_hh[idx(move)]) / (1+_score_bf[idx(move)]));
}

}