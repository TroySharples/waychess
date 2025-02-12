#pragma once

#include <vector>
#include <map>
#include <span>
#include <cstdint>
#include <cstddef>
#include <iostream>

class hashtable
{
public:
    hashtable() = default;
    hashtable(std::span<std::uint64_t> table, std::uint64_t magic, std::uint64_t rshift);

    // The function that is actually used to originally calculate the magic. This will take a while (minutes).
    // We append to buf in the process as the hash-maps table itself is non-owning.
    hashtable(std::vector<std::uint64_t>& buf, const std::map<std::uint64_t, std::uint64_t>& map);

    constexpr std::uint64_t operator[](std::uint64_t x) const noexcept { return _table[(x * _magic) >> _rshift]; }

    constexpr std::span<std::uint64_t> get_table() const noexcept { return _table; }
    constexpr std::uint64_t get_magic() const noexcept { return _magic; }
    constexpr std::uint64_t get_rshift() const noexcept { return _rshift; }

    constexpr std::uint64_t get_index_bits() const noexcept { return 64 - _rshift; }

private:
    std::span<std::uint64_t> _table;

    std::uint64_t _magic;
    // This is used instead of index-bits to save a subtraction during every lookup (I'm not
    // sure if the compiler would be clever enough to optimise this out otherwise).
    std::uint64_t _rshift;
};
