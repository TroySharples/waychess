#include "move.hpp"

#include "utility/coordinates.hpp"

namespace move
{

std::string to_algebraic_long(std::uint32_t move)
{
    std::string ret;

    const std::uint8_t from_mb { move::deserialise_from_mb(move) };
    const std::uint8_t to_mb   { move::deserialise_to_mb(move) };
    const std::uint8_t type    { move::deserialise_move_type(move) };
    const std::uint8_t info    { move::deserialise_move_info(move) };

    ret.append(to_coordinates_str(from_mb));
    ret.append(to_coordinates_str(to_mb));
    if (type & move::move_type::PROMOTION)
    {
        switch (info)
        {
            case move::move_info::PROMOTION_BISHOP: ret.push_back('b'); break;
            case move::move_info::PROMOTION_KNIGHT: ret.push_back('n'); break;
            case move::move_info::PROMOTION_QUEEN:  ret.push_back('q'); break;
            case move::move_info::PROMOTION_ROOK:   ret.push_back('r'); break;
        }
    }

    return ret;
}

}