#include "ram.hpp"

#include <array>
#include <new>
#include <stdexcept>

// We can mess with exactly how this RAM is allocated later. For now, we set it as one contiguous static array.
namespace
{

// The major components that are designed to be stored in this section are the sliding piece PEXT bitboard
// tables. These are at most 12-bits deep for rooks, and 9-bits deep for bishops, which ends up pessimistically
// as 64*(2**12 + 2**9) = 294912, which is just over 2**18. We allocate 2*19 bytes for safety - allocation
// errors are thrown at runtime if there isn't enough space anywy.
constexpr std::size_t RAM_ENTRIES { 1ULL << 20 };

std::array<std::uint64_t, RAM_ENTRIES> ram;
std::size_t offset { 0 };

}

namespace details
{

std::span<std::uint64_t> get_ram_slice(std::size_t entries)
{
    if (entries + offset > RAM_ENTRIES)
        throw std::bad_alloc();

    auto ret = std::span<std::uint64_t>(ram.data() + offset, entries);
    offset += entries;

    return ret;
}

}