#pragma once

#include "bitboard.hpp"

#include "random.hpp"

#include <algorithm>
#include <span>
#include <cassert>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <span>

// The first element is the magic multiplier, the second the number of index bits.
using magic = std::pair<std::uint64_t, std::size_t>;

constexpr std::uint64_t hasher(std::uint64_t x, magic m)
{
    return (m.first * x) >> (64 - m.second);
}

consteval magic magic_generator(std::span<const std::uint64_t> keys, std::size_t max_index_bits = 16)
{
    // The minimum possible index bits is log2 of the number of required unique keys.
    const std::size_t min_index_bits { log2_ceil(keys.size()) };
    if (min_index_bits > max_index_bits)
        throw std::runtime_error("Impossible maximum index bits");

    magic ret { CONSTEXPR_RANDOM, min_index_bits };

    // Keep increasing the index bits until we reach our maximum.
    for (; ret.second <= max_index_bits; ret.second++)
    {
        // We calculate the approximate probability that a given magic value will not lead to any collisions
        // for this index size.
        float success_p { 1.0 };
        for (std::size_t i = 0; i < keys.size(); i++)
            success_p *= (1.0 - float(i) / float(2 << ret.second));
        
        // Work out how many attempts we should make before giving up and increasing the index size. If this
        // number is rediculous we just increment straight away without trying.
        constexpr std::size_t MAX_EXPECTED_ATTEMPTS { 1000 };
        if (success_p < 1.0 / float(MAX_EXPECTED_ATTEMPTS))
            continue;
        const std::size_t expected_attempts_n = static_cast<std::size_t>(1.0 / success_p);

        for (std::size_t i = 0; i < 2*expected_attempts_n; i++)
        {
            // Generate the next random magic by hashing our current one - hopefully there are no fixed points!
            ret.first = constexpr_hash(ret.first);

            // Use std::vector to detect if we have any collisions - unfortunately none of the C++ hash-maps
            // support transient constexpr at the moment... This will do for now.
            std::vector<std::uint64_t> digests;
            for (auto x : keys)
                digests.push_back(hasher(x, ret));
            std::sort(digests.begin(), digests.end());
            if (std::unique(digests.begin(), digests.end()) == digests.end())
                return ret;
        }
    }

    throw std::runtime_error("Could not determine magic value in reasonable time");
}