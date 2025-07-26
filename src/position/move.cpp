#include "move.hpp"

#include "pieces/pieces.hpp"
#include "utility/coordinates.hpp"
#include "generate_moves.hpp"

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
    if (move::move_type::is_promotion(type))
        ret.push_back(to_fen_char(static_cast<piece_idx>(piece_idx::b_pawn | info)));

    return ret;
}

std::uint32_t from_algebraic_long(std::string_view algebraic, const bitboard& bb)
{
    // This is very hacky, but it's easy and works, and doesn't need to be fast.
    std::array<std::uint32_t, MAX_MOVES_PER_POSITION> move_buf;
    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
        if (const std::uint32_t move { move_buf[i] }; to_algebraic_long(move) == algebraic)
            return move;

    throw std::invalid_argument("Error deserialising move - no such move in position");
}

}