#pragma once

#include <iostream>
#include <cstdint>
#include <bit>

using bitboard = std::uint64_t;

// We index the ranks starting at 0 (i.e. the white first rank has index 0).
constexpr bitboard get_rank_squares(std::size_t i) noexcept
{
    return 0xffull << 8*i;
}

// We index the files starting at 0 (i.e. the a file has index 0).
constexpr bitboard get_file_squares(std::size_t i) noexcept
{
    return 0x0101010101010101ull << i;
}

constexpr bitboard WHITE_SQUARES { 0x55aa55aa55aa55aa };
constexpr bitboard BLACK_SQUARES { ~WHITE_SQUARES };

constexpr bitboard FILE_A { get_file_squares(0) };
constexpr bitboard FILE_B { get_file_squares(1) };
constexpr bitboard FILE_C { get_file_squares(2) };
constexpr bitboard FILE_D { get_file_squares(3) };
constexpr bitboard FILE_E { get_file_squares(4) };
constexpr bitboard FILE_F { get_file_squares(5) };
constexpr bitboard FILE_G { get_file_squares(6) };
constexpr bitboard FILE_H { get_file_squares(7) };

constexpr bitboard RANK_1 { get_rank_squares(0) };
constexpr bitboard RANK_2 { get_rank_squares(1) };
constexpr bitboard RANK_3 { get_rank_squares(2) };
constexpr bitboard RANK_4 { get_rank_squares(3) };
constexpr bitboard RANK_5 { get_rank_squares(4) };
constexpr bitboard RANK_6 { get_rank_squares(5) };
constexpr bitboard RANK_7 { get_rank_squares(6) };
constexpr bitboard RANK_8 { get_rank_squares(7) };

constexpr bitboard CENTRE_4_SQUARES  { (FILE_D | FILE_E)
                                          & (RANK_4 | RANK_5) };
constexpr bitboard CENTRE_16_SQUARES { (FILE_C | FILE_D | FILE_E | FILE_F)
                                          & (RANK_3 | RANK_4 | RANK_5 | RANK_6) };
constexpr bitboard RIM_SQUARES { FILE_A | FILE_H };

std::ostream& display(std::ostream& os, bitboard v);

constexpr bitboard ls1b_isolate(bitboard x) noexcept { return (~x+1) & x; }
constexpr bitboard ls1b_reset(bitboard x)  noexcept{ return (x-1) & x; }
constexpr std::size_t bitscan_forward(bitboard x) noexcept { return std::countr_zero(x); }
constexpr std::size_t bitscan_backward(bitboard x) noexcept { return std::countl_zero(x); }
constexpr std::size_t popcount(bitboard x) noexcept { return std::popcount(x); }
