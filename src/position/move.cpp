#include "move.hpp"

#include "pieces/pieces.hpp"
#include "utility/coordinates.hpp"

namespace move
{

std::string to_algebraic_long(std::uint32_t move) noexcept
{
    std::string ret;

    const std::uint8_t from_mb { move::deserialise_from_mb(move) };
    const std::uint8_t to_mb   { move::deserialise_to_mb(move) };
    const std::uint8_t type    { move::deserialise_move_type(move) };
    const std::uint8_t info    { move::deserialise_move_info(move) };

    ret.append(to_coordinates_str(from_mb));
    ret.append(to_coordinates_str(to_mb));
    if (type & move::move_type::PROMOTION)
        ret.push_back(to_fen_char(static_cast<piece_idx>(piece_idx::b_pawn | info)));

    return ret;
}

}