#include <span>
#include <cstdint>

namespace details
{

std::span<std::uint64_t> get_ram_slice(std::size_t entries);

}