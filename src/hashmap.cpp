#include "hashmap.hpp"

#include <stdexcept>
#include <set>
#include <cmath>
#include <random>
#include <bit>

void hashmap::insert(std::uint64_t key, std::uint64_t value)
{
    if (!_map.insert({ key, value }).second)
        throw std::runtime_error("Unable to insert element into map");
}

void hashmap::fix_map(std::uint64_t max_index_bits)
{
    // We try to take advantage of hash collisions to decrease the size of lookup table, so what we
    // are actually interested in is the number of unique values in the table.
    std::set<std::uint64_t> values;
    for (auto i : _map)
        values.insert(i.second);
    const std::size_t unique_values_n = values.size();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max()
    );

    _rshift = std::countl_zero(unique_values_n);
    for (; get_index_bits() < max_index_bits; _rshift--)
    {
        _table.resize(1 << get_index_bits());

        // We try for a number of times before giving up and increasing the number of index-bits.
        constexpr std::size_t MAX_ATTEMPTS { 5000 };
        for (std::size_t _ = 0; _ < MAX_ATTEMPTS; _++)
        {
            _magic = dis(gen);

            // Load up the vector.
            for (auto [ key, value ] : _map)
                _table[(key * _magic) >> _rshift] = value;

            // Check to see if this magic works.
            bool success { true };
            for (auto [ key, value ] : _map)
            {
                if (this->operator()(key) != value)
                {
                    success = false;
                    break;
                }
            }
            if (success)
                return;
        }
    }

    // We weren't able to find a solution in reasonable time.
    throw std::runtime_error("Unable to calculate hashmap parameters");
}