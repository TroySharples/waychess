#pragma once

#include <span>
#include <bit>
#include <cstdint>

namespace details
{

// TODO: Verify that attribute-packed doesn't impact performance here.
template <typename T>
class hash_table
{
public:
    using key_type = std::uint64_t;

    // The table also stores the full key so type-2 collisions can be detected (comparison
    // with original key).
    struct entry_type
    {
        using value_type = T;

        key_type key;
        value_type value;
    };

    using table_type = std::span<entry_type>;

    // Automatically calculates the key-mask from the table-size.
    hash_table() = default;
    hash_table(table_type table) noexcept;

    const entry_type& operator[](std::uint64_t x) const noexcept { return table[x & key_mask]; }
    entry_type& operator[](std::uint64_t x) noexcept { return table[x & key_mask]; }

    // Helpers getting information about the table.
    std::size_t get_table_entries() const noexcept { return table.size(); }
    std::size_t get_table_bytes()   const noexcept { return get_table_entries()*sizeof(entry_type); }

private:
    key_type key_mask;
    table_type table;
};

template <typename T>
inline hash_table<T>::hash_table(table_type table) noexcept
    : key_mask(std::bit_floor(table.size())-1), table(table)
{

}

}