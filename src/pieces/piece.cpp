#include "piece.hpp"

#include <immintrin.h>

const uint64_t& bitboard_lut::operator[](bitboard x) const noexcept
{ 
    return table[_pext_u64(x, mask)];
}

uint64_t& bitboard_lut::operator[](bitboard x) noexcept
{ 
    return table[_pext_u64(x, mask)];
}