#pragma once

#include <cstdint>
#include <bit>
#include <cstddef>

// Helper log2_ceil (I know we can use countl_zero but this is clearer).
constexpr std::uint64_t log2_ceil(std::uint64_t x) noexcept
{
    std::size_t ret {};
    for (; x > 1; x /= 2)
        ret++;
    return ret;
}

// Constexpr 64-bit integer hasher.
constexpr std::uint64_t constexpr_hash(std::uint64_t x) noexcept
{
    // Generated with OpenSSL.
    constexpr std::uint64_t r[] {
        0x6db2223e62bbe8d9,
        0x04a227ca5e442910,
        0x290c526fba7858e1,
        0x0e0e57530f9a9b23,
        0x101548684ef8e25d,
        0x472e954be3552485,
        0xf66f115b483c2b7e,
        0x7a8e568796d3522a,
        0x7679445fb0a2223e,
        0x3e2f451b27d3aa0a,
        0xdfe8da1f1f6796d0,
        0xc3b74de976d8eb35,
        0xdc4863418a83a46b,
        0x2ad7239dd1ceca6c,
        0x10fbb709892fc796,
        0x82ac721cffce0219
    };
    std::uint64_t ret{ x ^ r[x & 0xF] };
    ret ^= std::rotl(x, 020) ^ r[(x >> 010) & 0xF];
    ret ^= std::rotl(x, 010) ^ r[(x >> 020) & 0xF];
    ret ^= std::rotr(x, 010) ^ r[(x >> 030) & 0xF];
    return ret;
}

// Constexpr string hasher - uses the integer one as a primitive.
template <std::size_t N>
constexpr std::uint64_t constexpr_hash(const char (&str)[N])
{
    std::uint64_t h {};
    for (std::size_t i = 0; i < N; i++)
        h = std::rotr(h, 8) ^ static_cast<std::uint64_t>(str[i]);
    return constexpr_hash(h);
}

// We need a way of generating compile-time random numbers for consteval magic generators.
// This is a hacky way that uses the __TIME__, __LINE__, and __FILE__ macros, inspired by
// KoneLinx on Github.
#define CONSTEXPR_RANDOM constexpr_hash(constexpr_hash(__TIME__) ^ constexpr_hash(__LINE__) ^ constexpr_hash(__FILE__))