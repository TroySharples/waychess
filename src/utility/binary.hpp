#pragma once

#include <iostream>
#include <cstdint>
#include <bit>
#include <vector>

// We index the ranks starting at 0 (i.e. the white first rank has index 0).
constexpr std::uint64_t get_bitboard_rank(std::size_t i) noexcept { return 0xffULL << 8*i; }

// We index the files starting at 0 (i.e. the a file has index 0).
constexpr std::uint64_t get_bitboard_file(std::size_t i) noexcept { return 0x0101010101010101ULL << i; }

// Get the bitboard represented by a single piece at this mailbox address.
constexpr std::uint64_t get_bitboard_mailbox_piece(std::size_t x) noexcept {  return 0b1ULL << x; }

// Get the bitboard represented by the rank of the square at this mailbox address.
constexpr std::uint64_t get_bitboard_mailbox_rank(std::size_t x) noexcept { return 0xffULL << (x & ~0x7ULL); }

// Get the bitboard represented by the file of the square at this mailbox address.
constexpr std::uint64_t get_bitboard_mailbox_file(std::size_t x) noexcept { return std::rotl(0x0101010101010101ULL, x); }

constexpr std::uint64_t WHITE_SQUARES { 0x55aa55aa55aa55aa };
constexpr std::uint64_t BLACK_SQUARES { ~WHITE_SQUARES };

constexpr std::uint64_t FILE_A { get_bitboard_file(0) };
constexpr std::uint64_t FILE_B { get_bitboard_file(1) };
constexpr std::uint64_t FILE_C { get_bitboard_file(2) };
constexpr std::uint64_t FILE_D { get_bitboard_file(3) };
constexpr std::uint64_t FILE_E { get_bitboard_file(4) };
constexpr std::uint64_t FILE_F { get_bitboard_file(5) };
constexpr std::uint64_t FILE_G { get_bitboard_file(6) };
constexpr std::uint64_t FILE_H { get_bitboard_file(7) };

constexpr std::uint64_t RANK_1 { get_bitboard_rank(0) };
constexpr std::uint64_t RANK_2 { get_bitboard_rank(1) };
constexpr std::uint64_t RANK_3 { get_bitboard_rank(2) };
constexpr std::uint64_t RANK_4 { get_bitboard_rank(3) };
constexpr std::uint64_t RANK_5 { get_bitboard_rank(4) };
constexpr std::uint64_t RANK_6 { get_bitboard_rank(5) };
constexpr std::uint64_t RANK_7 { get_bitboard_rank(6) };
constexpr std::uint64_t RANK_8 { get_bitboard_rank(7) };

constexpr std::uint64_t CENTRE_4_SQUARES  { (FILE_D | FILE_E)
                                          & (RANK_4 | RANK_5) };
constexpr std::uint64_t CENTRE_16_SQUARES { (FILE_C | FILE_D | FILE_E | FILE_F)
                                          & (RANK_3 | RANK_4 | RANK_5 | RANK_6) };
constexpr std::uint64_t RIM_SQUARES { FILE_A | FILE_H };

constexpr std::uint64_t shift_south_west(std::uint64_t x) noexcept { return (x >> 9) & ~FILE_H; }
constexpr std::uint64_t shift_west(std::uint64_t x) noexcept { return (x >> 1) & ~FILE_H; }
constexpr std::uint64_t shift_north_west(std::uint64_t x) noexcept { return (x << 7) & ~FILE_H; }
constexpr std::uint64_t shift_north(std::uint64_t x) noexcept { return x << 8; }
constexpr std::uint64_t shift_north_east(std::uint64_t x) noexcept { return (x << 9) & ~FILE_A; }
constexpr std::uint64_t shift_east(std::uint64_t x) noexcept { return (x << 1) & ~FILE_A; }
constexpr std::uint64_t shift_south_east(std::uint64_t x) noexcept { return (x >> 7) & ~FILE_A; }
constexpr std::uint64_t shift_south(std::uint64_t x) noexcept { return x >> 8; }

std::ostream& display(std::ostream& os, std::uint64_t v);

constexpr std::uint64_t ls1b_isolate(std::uint64_t x) noexcept { return (~x+1) & x; }
constexpr std::uint64_t ls1b_reset(std::uint64_t x)  noexcept{ return (x-1) & x; }
constexpr std::size_t bitscan_forward(std::uint64_t x) noexcept { return std::countr_zero(x); }
constexpr std::size_t bitscan_backward(std::uint64_t x) noexcept { return std::countl_zero(x); }
constexpr std::size_t popcount(std::uint64_t x) noexcept { return std::popcount(x); }

// Takes a bitboard and emits a vector of size 2**popcount(x) containing all possible
// 0 and 1 combinations of the elements of the bitboard with set bits.
std::vector<std::uint64_t> get_1s_combinations(std::uint64_t x);
