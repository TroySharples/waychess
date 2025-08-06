#include "position/search.hpp"

#include <gtest/gtest.h>
#include <fstream>

namespace
{

struct puzzle
{
    bitboard bb;
    std::vector<std::uint32_t> moves;

    friend std::istream& operator>>(std::istream& is, puzzle& v)
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
};

}

TEST(Puzzles, Lichess5000)
{
    std::size_t puzzles_total {};
    std::size_t puzzles_solved {};

    std::ifstream is(PUZZLE_CSV_PATH);
    if (!is)
        throw std::runtime_error("Unable to open puzzle CSV file");

    // Absorb the one-line header of column labels.
    std::string token;
    if (!getline(is, token))
        throw std::runtime_error("Unable to parse CSV column header");

    while (getline(is, token))
    {
        std::stringstream ss(token);

        puzzle p;
        ss >> p;

        bool puzzle_solved { true };
        bool is_to_move    { false };
        for (auto move : p.moves)
        {

            if (is_to_move)
            {
                constexpr std::size_t depth { 4 };
                const auto best_move = best_move_minimax(p.bb, depth, &evaluate_terminal);
                if (best_move.first != move)
                {
                    puzzle_solved = false;
                    break;
                }
            }

            make_move({ .check_legality = false }, p.bb, move);
            is_to_move = !is_to_move;
        }

        puzzles_total++;
        if (puzzle_solved)
            puzzles_solved++;
    }

    const double accuracy = static_cast<double>(puzzles_solved)/puzzles_total;

    std::cout << "puzzles solved - " << puzzles_solved << '\n';
    std::cout << "puzzles total  - " << puzzles_total << '\n';
    std::cout << "accuracy       - " << accuracy*100 << '\n';

    EXPECT_GE(accuracy, 0.6);
}

// Tests performannt perft correctness across a variety of positions.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
