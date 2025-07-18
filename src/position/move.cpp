#include "move.hpp"

#include "utility/coordinates.hpp"

namespace move
{

std::string to_algebraic_long(std::uint32_t move)
{
    std::string ret;

    const std::uint8_t from_mb { move::deserialise_from_square(move) };
    const std::uint8_t to_mb { move::deserialise_to_square(move) };
    const std::uint16_t type { move::deserialise_type(move) };

    ret.append(to_coordinates_str(from_mb));
    ret.append(to_coordinates_str(to_mb));
    if (type & move::move_type::PROMOTION)
    {
        if (type & move::move_type::PROMOTION_BISHOP)
            ret.push_back('B');
        else if (type & move::move_type::PROMOTION_KNIGHT)
            ret.push_back('N');
        else if (type & move::move_type::PROMOTION_QUEEN)
            ret.push_back('Q');
        else if (type & move::move_type::PROMOTION_ROOK)
            ret.push_back('R');
    }

    return ret;
}

}