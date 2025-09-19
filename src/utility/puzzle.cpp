#include "puzzle.hpp"

#include "search/search.hpp"

bool solver::solve(const puzzle& p, std::size_t depth)
{
    gs.reset();
    gs.load(p.bb);
    bool is_to_move { false };

    for (auto move : p.moves)
    {
        // If it is our side to move we have to run the search.
        if (is_to_move)
        {
            const search::recommendation rec { search::recommend_move(gs, depth) };

            // Return true early if we've found checkmate (there might be multiple winning mates in this position, so
            // otherwise we'd fail the test).
            if (std::abs(rec.eval) >= evaluation::EVAL_CHECKMATE)
                return true;

            // If there is no mate, and it is us to move, we just make sure we play the right move.
            if (move != rec.move)
                return false;
        }

        // Actually make the move.
        is_to_move = !is_to_move;
        make_move({ .check_legality = false }, gs, move);
    }

    return true;
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