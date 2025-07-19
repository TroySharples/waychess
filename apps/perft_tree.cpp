#include <cstdlib>
#include <stdexcept>
#include <string>

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"
#include "utility/perft.hpp"

int main(int argc, char** argv)
{
    const std::string usage_str { std::string("    Usage - ") + argv[0] + "[depth] [fen]" };

    // Verify correct parameters.
    if (argc != 3)
    {
        std::cerr << "Incorrect number of parameters:\n" << usage_str << std::endl;
        return EXIT_FAILURE;
    }

    // Parse inputs.
    const std::size_t depth { std::stoull(argv[1]) };
    if (depth == 0)
    {
        std::cerr << "Depth must be a positive integer:\n" << usage_str << std::endl;
        return EXIT_FAILURE;
    }
    const char* const fen { argv[2] };

    // Generate the first level of the tree manually and print the results.
    std::size_t total_moves {};

    const bitboard start(fen);
    for (auto move : generate_pseudo_legal_moves(start))
    {
        bitboard next_position { start };

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move))
            continue;

        const std::size_t moves { perft(next_position, depth-1) };
        total_moves += moves;

        std::cout << move::to_algebraic_long(move) << ' ' << moves << std::endl;
    }

    // Finally print the total number of moves.
    std::cout << "\n" << total_moves << std::endl;

    return EXIT_SUCCESS;
}