#pragma once

#include <span>
#include <cstdint>
#include <immintrin.h>

// Move generation for sliding pieces needs to take into account the position of other pieces on the board
// (i.e. blocking pieces). We do this using PEXT bitboards (all machines we are running this on will have
// the BMI2 instruction set). We did originally try this with fancy-black-magic bitboards and have a separate
// application that spent a lot of time calculating the magics. This implementation is a lot simpler and
// should also be faster (although not quite as space-efficient). Note that the underlying tables for these
// bitboards are non-owning to allow for more flexible RAM layouts in the future.

namespace details
{

struct pext_bitboard
{
    std::span<std::uint64_t> table;
    std::uint64_t mask;
    const std::uint64_t& operator[](std::uint64_t x) const noexcept { return table[_pext_u64(x, mask)];}
    std::uint64_t& operator[](std::uint64_t x) noexcept { return table[_pext_u64(x, mask)]; }
};

}