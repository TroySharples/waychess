#include "bitboard.hpp"

std::ostream& display(std::ostream& os, bitboard v)
{
    for (std::size_t rank = 0; rank < 8; rank++)
    {
        for (std::size_t file = 0; file < 8; file++)
            os << ((v >> ((7-rank)*8+file)) & 0x01) << ' ';
        os << '\n';
    }
    return os;
}

std::vector<bitboard> get_1s_combinations(bitboard x)
{
    std::vector<bitboard> ret;

    // Create a vector of the mailbox indicies of the 1-bits.
    std::vector<std::size_t> addrs;
    for (bitboard i = x; i != 0; i = ls1b_reset(i))
        addrs.push_back(std::countr_zero(i));
    const std::size_t addrs_bits { addrs.size() };

    // Return immediately if there are no 1s.
    if (addrs_bits == 0)
        return ret;

    // Count up in binary to emumerate all the combinations.
    const std::size_t combinations { 0b1ull << addrs_bits };
    ret.resize(combinations);
    for (std::size_t i = 0; i < combinations; i++)
        for (std::size_t j = 0; j < addrs_bits; j++)
            if ((i >> j) & 0b1)
                ret[i] |= (0b1ull << addrs[j]);
    
    return ret;
}