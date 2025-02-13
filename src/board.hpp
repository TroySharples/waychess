#pragma once

#include <iostream>
#include <cstdint>
#include <bit>
#include <vector>

using mailbox = std::size_t;
using bitboard = std::uint64_t;

// We index the ranks starting at 0 (i.e. the white first rank has index 0).
constexpr bitboard get_bitboard_rank(std::size_t i) noexcept { return 0xffULL << 8*i; }

// We index the files starting at 0 (i.e. the a file has index 0).
constexpr bitboard get_bitboard_file(std::size_t i) noexcept { return 0x0101010101010101ULL << i; }

// Get the bitboard represented by a single piece at this mailbox address.
constexpr bitboard get_bitboard_mailbox_piece(mailbox x) noexcept {  return 0b1ULL << x; }

// Get the bitboard represented by the rank of the square at this mailbox address.
constexpr bitboard get_bitboard_mailbox_rank(mailbox x) noexcept { return 0xffULL << (x & ~0x7ULL); }

// Get the bitboard represented by the file of the square at this mailbox address.
constexpr bitboard get_bitboard_mailbox_file(mailbox x) noexcept { return std::rotl(0x0101010101010101ULL, x); }

constexpr bitboard WHITE_SQUARES { 0x55aa55aa55aa55aa };
constexpr bitboard BLACK_SQUARES { ~WHITE_SQUARES };

constexpr bitboard FILE_A { get_bitboard_file(0) };
constexpr bitboard FILE_B { get_bitboard_file(1) };
constexpr bitboard FILE_C { get_bitboard_file(2) };
constexpr bitboard FILE_D { get_bitboard_file(3) };
constexpr bitboard FILE_E { get_bitboard_file(4) };
constexpr bitboard FILE_F { get_bitboard_file(5) };
constexpr bitboard FILE_G { get_bitboard_file(6) };
constexpr bitboard FILE_H { get_bitboard_file(7) };

constexpr bitboard RANK_1 { get_bitboard_rank(0) };
constexpr bitboard RANK_2 { get_bitboard_rank(1) };
constexpr bitboard RANK_3 { get_bitboard_rank(2) };
constexpr bitboard RANK_4 { get_bitboard_rank(3) };
constexpr bitboard RANK_5 { get_bitboard_rank(4) };
constexpr bitboard RANK_6 { get_bitboard_rank(5) };
constexpr bitboard RANK_7 { get_bitboard_rank(6) };
constexpr bitboard RANK_8 { get_bitboard_rank(7) };

constexpr bitboard CENTRE_4_SQUARES  { (FILE_D | FILE_E)
                                          & (RANK_4 | RANK_5) };
constexpr bitboard CENTRE_16_SQUARES { (FILE_C | FILE_D | FILE_E | FILE_F)
                                          & (RANK_3 | RANK_4 | RANK_5 | RANK_6) };
constexpr bitboard RIM_SQUARES { FILE_A | FILE_H };


constexpr bitboard shift_south_west(bitboard x) noexcept { return (x >> 9) & ~FILE_H; }
constexpr bitboard shift_west(bitboard x) noexcept { return (x >> 1) & ~FILE_H; }
constexpr bitboard shift_north_west(bitboard x) noexcept { return (x << 7) & ~FILE_H; }
constexpr bitboard shift_north(bitboard x) noexcept { return x << 8; }
constexpr bitboard shift_north_east(bitboard x) noexcept { return (x << 9) & ~FILE_A; }
constexpr bitboard shift_east(bitboard x) noexcept { return (x << 1) & ~FILE_A; }
constexpr bitboard shift_south_east(bitboard x) noexcept { return (x >> 7) & ~FILE_A; }
constexpr bitboard shift_south(bitboard x) noexcept { return x >> 8; }

std::ostream& display(std::ostream& os, bitboard v);

constexpr bitboard ls1b_isolate(bitboard x) noexcept { return (~x+1) & x; }
constexpr bitboard ls1b_reset(bitboard x)  noexcept{ return (x-1) & x; }
constexpr std::size_t bitscan_forward(bitboard x) noexcept { return std::countr_zero(x); }
constexpr std::size_t bitscan_backward(bitboard x) noexcept { return std::countl_zero(x); }
constexpr std::size_t popcount(bitboard x) noexcept { return std::popcount(x); }

// Takes a bitboard and emits a vector of size 2**popcount(x) containing all possible
// 0 and 1 combinations of the elements of the bitboard with set bits.
std::vector<bitboard> get_1s_combinations(bitboard x);
