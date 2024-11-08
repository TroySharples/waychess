#include "position.hpp"

std::ostream& display(std::ostream& os, std::uint64_t v)
{
    for (std::size_t rank = 0; rank < 8; rank++)
    {
        for (std::size_t file = 0; file < 8; file++)
            os << ((v >> ((7-rank)*8+file)) & 0x01) << ' ';
        os << '\n';
    }
    return os;
}