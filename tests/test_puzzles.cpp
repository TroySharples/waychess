#include "utility/puzzle.hpp"
#include "position/search.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace
{

void test_puzzles(const std::filesystem::path& csv_path, double pass_accuracy)
{
    std::size_t puzzles_total {};
    std::size_t puzzles_solved {};

    std::ifstream is(csv_path);
    ASSERT_TRUE(is);

    std::string line;
    ASSERT_TRUE(std::getline(is, line));

    const recommendation rec = [] (const bitboard& bb) {
        constexpr std::uint8_t depth { 4 };
        return best_move_minimax(bb, depth, &evaluate_terminal).first;
    };

    while (std::getline(is, line))
    {
        std::stringstream ss(line);

        puzzle p;
        ss >> p;

        puzzles_total++;
        if (p.solve(rec)) puzzles_solved++;
    }

    const double accuracy = static_cast<double>(puzzles_solved) / static_cast<double>(puzzles_total);

    EXPECT_GE(accuracy, pass_accuracy);
}

}

TEST(Puzzle, Mate500)
{
    test_puzzles(std::filesystem::path(PUZZLE_DIR) / "mate500.csv", 0.9);
}

TEST(Puzzle, Endgame500)
{
    test_puzzles(std::filesystem::path(PUZZLE_DIR) / "endgame500.csv", 0.5);
}

// Tests the rough performance of the search and evaluation by evaluate puzzle-solving
// performance across a variety of position types.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}