#pragma once

#include "board.hpp"
#include "hashtable.hpp"

#include <iostream>
#include <array>

// The attack tables for jumping pieces can be entirely represented by an array of pre-calculated bitboards.

using bitboard_table = std::array<bitboard, 64>;

// Move generation for sliding pieces needs to take into account the position of other pieces on the board (i.e. blocking pieces). We use
// the fancy black-magic bitboards implementation to quickly calculate this.

struct smagic
{
    hashtable table;
    bitboard notmask;
    uint64_t operator[](bitboard x) const noexcept { return table[notmask | x]; }
};

using sliding_attack_table = std::array<smagic, 64>;

std::ostream& operator<<(std::ostream& os, const sliding_attack_table& v);
