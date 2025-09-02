#include "puzzle.hpp"

#include "position/make_move.hpp"

#include <sstream>

bool puzzle::solve(std::uint8_t max_depth, evaluation::evaluation eval) const
{
    bool ret { true };

    game_state gs   { bb };
    bool is_to_move { false };

    for (auto move : moves)
    {
        if (is_to_move && move != search::recommend_move(gs, max_depth, eval).move)
            ret = false;

        make_move({ .check_legality = false }, gs, move);

        // Increment the actual root ply.
        gs.root_ply++;

        is_to_move = !is_to_move;
    }

    return ret;
}

std::istream& operator>>(std::istream& is, puzzle& v)
{
    std::string token;

    // Reads past the puzzle-id.
    if (!getline(is, token, ','))
        throw std::runtime_error("Unable to parse puzzle-id");

    // Read the FEN.
    if (!getline(is, token, ','))
        throw std::runtime_error("Unable to parse FEN");
    v.bb = bitboard(token);

    // Read the moves list.
    if (!getline(is, token, ','))
        throw std::runtime_error("Unable to parse move-list");

    // Parse the space-separated moves list into individual moves.
    {
        bitboard bb_copy { v.bb };

        std::stringstream ss(token);
        while (getline(ss, token, ' '))
        {
            const std::uint32_t move { move::from_algebraic_long(token, bb_copy) };
            make_move({ .check_legality = false }, bb_copy, move);

            v.moves.push_back(move);
        }
    }

    // Discard the rest of the puzzle info and read to the end of the line.
    if (!getline(is, token))
        throw std::runtime_error("Unable to parse rest of puzzle");

    return is;
}