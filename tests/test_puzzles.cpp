#include "utility/puzzle.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace
{

// Global puzzle solver.
solver s;

void test_puzzles(const std::filesystem::path& csv_path, std::size_t depth, double pass_accuracy)
{
    std::size_t puzzles_total {};
    std::size_t puzzles_solved {};

    std::ifstream is(csv_path);
    ASSERT_TRUE(is);

    std::string line;
    ASSERT_TRUE(std::getline(is, line));

    while (std::getline(is, line))
    {
        std::stringstream ss(line);

        solver::puzzle p;
        ss >> p;

        puzzles_total++;
        if (s.solve(p, depth))
            puzzles_solved++;
    }

    const double accuracy = static_cast<double>(puzzles_solved) / static_cast<double>(puzzles_total);

    EXPECT_GE(accuracy, pass_accuracy);
}

}

TEST(Puzzle, Mate500)
{
    test_puzzles(std::filesystem::path(PUZZLE_DIR) / "mate500.csv", 9, 1.0);
}

TEST(Puzzle, Endgame500)
{
    test_puzzles(std::filesystem::path(PUZZLE_DIR) / "endgame500.csv", 9, 0.85);
}

TEST(Puzzle, Middlegame500)
{
    test_puzzles(std::filesystem::path(PUZZLE_DIR) / "middlegame500.csv", 7, 0.85);
}

// Tests the rough performance of the search and evaluation by evaluate puzzle-solving
// performance across a variety of position types.
int main(int argc, char **argv)
{
    // Allocate 128 MB for our transposition table.
    s.gs.tt.set_table_bytes(128*1000000ULL);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}