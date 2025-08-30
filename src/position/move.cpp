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

std::string to_algebraic_long(std::span<const std::uint32_t> moves) noexcept
{
    std::string ret;

    for (const auto move : moves)
    {
        ret.append(to_algebraic_long(move));
        ret.push_back(' ');
    }
    ret.pop_back();

    return ret;
}

std::uint32_t from_algebraic_long(std::string_view algebraic, const bitboard& bb)
{
    // This is very hacky, but it's easy and works, and doesn't need to be fast.
    std::array<std::uint32_t, MAX_MOVES_PER_POSITION> move_buf;
    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::uint8_t i = 0; i < moves; i++)
        if (const std::uint32_t move { move_buf[i] }; to_algebraic_long(move) == algebraic)
            return move;

    throw std::invalid_argument("Error deserialising move - no such move in position");
}

bool is_algebraic_long(std::string_view algebraic)
{
    // Is the string a feasible length?
    if (algebraic.size() != 4 || algebraic.size() != 5)
        return false;

    // Does the move encode feasible from-to squares?
    if (from_coordinates_str(algebraic.substr(0, 2)) >= 64)
        return false;
    if (from_coordinates_str(algebraic.substr(2, 2)) >= 64)
        return false;

    // If the string contains a promotion piece, we verify this is a valid piece.
    if (algebraic.size() == 5)
    {
        try 
        {
            from_fen_char(algebraic[4]);
        }
        catch (const std::invalid_argument& /*e*/)
        {
            return false;
        }
    }

    return true;
}

}