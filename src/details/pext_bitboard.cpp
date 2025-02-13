#include "pext_bitboard.hpp"

#include <immintrin.h>

namespace details
{

const std::uint64_t& pext_bitboard::operator[](std::uint64_t x) const noexcept
{ 
    return table[_pext_u64(x, mask)];
}

std::uint64_t& pext_bitboard::operator[](std::uint64_t x) noexcept
{ 
    return table[_pext_u64(x, mask)];
}

}