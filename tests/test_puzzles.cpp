#include "utility/puzzle.hpp"
#include "position/search.hpp"

#include <gtest/gtest.h>
#include <fstream>

TEST(Puzzles, Mate5000)
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

    const recommendation rec = [] (const bitboard& bb) { 
        constexpr std::uint8_t depth { 4 };

        return best_move_minimax(bb, depth, &evaluate_terminal).first;
    };

    while (getline(is, token))
    {
        std::stringstream ss(token);

        puzzle p;
        ss >> p;

        puzzles_total++;
        if (p.solve(rec)) puzzles_solved++;
    }

    const double accuracy = static_cast<double>(puzzles_solved)/static_cast<double>(puzzles_total);

    std::cout << "puzzles total  - " << puzzles_total << '\n';
    std::cout << "puzzles solved - " << puzzles_solved << '\n';
    std::cout << "accuracy       - " << accuracy*100 << '\n';

    EXPECT_GE(accuracy, 0.6);
}

// Tests performannt perft correctness across a variety of positions.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
