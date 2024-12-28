#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include <cstddef>

class hashmap
{
public:
    constexpr std::uint64_t operator()(std::uint64_t x) const noexcept { return _table[(x * _magic) >> _rshift]; }

    constexpr std::uint64_t get_index_bits() const noexcept { return 64 - _rshift; }

    // These methods are used to load up the hash-map, and to fix it once all the elements are in place.
    void insert(std::uint64_t key, std::uint64_t value);
    void fix_map(std::uint64_t max_index_bits = 16);

private:
    std::vector<std::uint64_t> _table;

    std::uint64_t _magic;
    // This is used instead of index-bits to save a subtraction during every lookup (I'm not
    // sure if the compiler would be clever enough to optimise this out otherwise).
    std::uint64_t _rshift;

    // Used for storing up the key-value pairs before calculating the magic and the index-bits.
    std::map<std::uint64_t, std::uint64_t> _map;
};
