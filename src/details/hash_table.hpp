#pragma once

#include <vector>
#include <bit>
#include <cstdint>

namespace details
{

// ####################################
// DECLARATION
// ####################################

template <typename T>
class hash_table
{
public:
    using key_type = std::uint64_t;

    // The table also stores the full key so type-2 collisions can be detected (comparison
    // with original key).
    struct __attribute__ ((__packed__)) entry_type
    {
        using value_type = T;

        key_type key;
        value_type value;
    };

    const entry_type& operator[](std::uint64_t x) const noexcept { return _table[x & _key_mask]; }
    entry_type& operator[](std::uint64_t x) noexcept { return _table[x & _key_mask]; }

    // Getters and setters of the table-size in number of entries.
    std::size_t get_table_entries() const noexcept;
    void set_table_entries(std::size_t entries);

    // Getters and setters of the table-size in bytes.
    std::size_t get_table_bytes() const noexcept;
    void set_table_bytes(std::size_t bytes);

private:
    key_type _key_mask;
    std::vector<entry_type> _table;
};

// ####################################
// IMPLEMENTATION
// ####################################

template <typename T>
inline std::size_t hash_table<T>::get_table_entries() const noexcept
{
    return _table.size();
}

template <typename T>
inline void hash_table<T>::set_table_entries(std::size_t entries)
{
    // We can only allocate the log2-floor of the requested entries because of how our mask works. The use should then
    // call get_table_entries for the true number of entries allocated.
    _table.resize(std::bit_floor(entries));
    _table.shrink_to_fit();
    _key_mask = std::bit_floor(_table.size())-1; 
}

template <typename T>
inline std::size_t hash_table<T>::get_table_bytes() const noexcept
{
    return get_table_entries()*sizeof(entry_type);
}

template <typename T>
inline void hash_table<T>::set_table_bytes(std::size_t bytes)
{
    set_table_entries(bytes/sizeof(entry_type));
}

}