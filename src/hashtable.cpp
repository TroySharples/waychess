#include "hashtable.hpp"

#include <immintrin.h>

#include <stdexcept>
#include <set>
#include <cmath>
#include <bit>

// A quick non-secure way of generating 64-bit random numbers.
static std::uint64_t random_u64()
{
    const std::int64_t ret = (static_cast<std::int64_t>(rand()) << 32) | static_cast<std::int64_t>(rand());
    return reinterpret_cast<const std::uint64_t&>(ret);
}

hashtable::hashtable(std::vector<std::uint64_t>& buf, const std::map<std::uint64_t, std::uint64_t>& map)
{
    // We try to take advantage of hash collisions to decrease the size of lookup table, so what we
    // are actually interested in is the number of unique values in the table.
    std::set<std::uint64_t> values;
    for (auto i : map)
        values.insert(i.second);
    const std::size_t unique_values_n = values.size();

    _rshift = std::countl_zero(unique_values_n)+1;
    for (; _rshift > 0; _rshift--)
    {
        buf.resize(1 << get_index_bits());

        // We try for a number of times before giving up and increasing the number of index-bits.
        constexpr std::size_t MAX_ATTEMPTS { 1000000 };
        for (std::size_t _ = 0; _ < MAX_ATTEMPTS; _++)
        {
            _magic = random_u64();

            // Load up the vector.
            for (auto [ key, value ] : map)
                buf[(key * _magic) >> _rshift] = value;

            // Check to see if this magic works.
            bool success { true };
            _table = buf;
            for (auto [ key, value ] : map)
            {
                if (operator[](key) != value)
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
