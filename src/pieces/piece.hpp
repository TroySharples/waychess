#pragma once

#include "board.hpp"

#include <span>
#include <array>

// The attack tables for jumping pieces can be entirely represented by an array of pre-calculated bitboards.

using bitboard_table = std::array<bitboard, 64>;

// Move generation for sliding pieces needs to take into account the position of other pieces on the board
// (i.e. blocking pieces). We do this using PEXT bitboards (all machines we are running this on will have
// the BMI2 instruction set). We did originally try this with fancy-black-magic bitboards and have a separate
// application that spent a lot of time calculating the magics. This implementation is a lot simpler and
// should also be faster (although not quite as space-efficient). Note that the underlying tables for these
// bitboards are non-owning, to allow for more flexible RAM layouts in the future.

struct bitboard_lut
{
    std::span<std::uint64_t> table;
    bitboard mask;
    const uint64_t& operator[](bitboard x) const noexcept;
    uint64_t& operator[](bitboard x) noexcept;
};

using sliding_attack_table = std::array<bitboard_lut, 64>;

std::ostream& operator<<(std::ostream& os, const sliding_attack_table& v);
