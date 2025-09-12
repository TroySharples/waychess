#include "puzzle.hpp"

#include "search/search.hpp"

bool solver::solve(const puzzle& p, std::size_t depth)
{
    bool ret { true };

    gs.load(p.bb);
    bool is_to_move { false };

    for (auto move : p.moves)
    {
        if (is_to_move && move != search::recommend_move(gs, depth).move)
            ret = false;

        make_move({ .check_legality = false }, gs, move);

        is_to_move = !is_to_move;
    }

    return ret;
}

std::istream& operator>>(std::istream& is, solver::puzzle& v)
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