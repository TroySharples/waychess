#pragma once

#include <iostream>
#include <cstdint>

// We index the ranks starting at 0 (i.e. the white first rank has index 0).
constexpr std::uint64_t get_rank_squares(std::size_t i) noexcept
{
    return 0xffull << 8*i;
}

// We index the files starting at 0 (i.e. the a file has index 0)
constexpr std::uint64_t get_file_squares(std::size_t i) noexcept
{
    return 0x0101010101010101ull << i;
}

constexpr std::uint64_t WHITE_SQUARES { 0x55aa55aa55aa55aa };
constexpr std::uint64_t BLACK_SQUARES { ~WHITE_SQUARES };

constexpr std::uint64_t CENTRE_4_SQUARES  { (get_rank_squares(3) | get_rank_squares(4)) 
                                          & (get_file_squares(3) | get_file_squares(4)) };
constexpr std::uint64_t CENTRE_16_SQUARES { (get_rank_squares(2) | get_rank_squares(3) | get_rank_squares(4) | get_rank_squares(5))
                                          & (get_file_squares(2) | get_file_squares(3) | get_file_squares(4) | get_file_squares(5)) };
constexpr std::uint64_t RIM_SQUARES { get_file_squares(0) | get_file_squares(7) };

std::ostream& display(std::ostream& os, std::uint64_t v);