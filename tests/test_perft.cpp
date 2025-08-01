#include "utility/perft.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

TEST(Perft, StartingPosition)
{
    std::vector<std::size_t> results {
        1,
        20,
        400,
        8902,
        197281,
        4865609,
        119060324,
        3195901860
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft({ .depth=i }, bitboard(STARTING_FEN)), results[i]);
}

TEST(Perft, Kiwipete)
{
    std::vector<std::size_t> results {
        1,
        48,
        2039,
        97862,
        4085603,
        193690690,
        8031647685
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft({ .depth=i }, bitboard(KIWIPETE_FEN)), results[i]);
}

TEST(Perft, Pos3)
{
    std::vector<std::size_t> results {
        1,
        14,
        191,
        2812,
        43238,
        674624,
        11030083,
        178633661,
        3009794393
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft({ .depth=i }, bitboard(POS3_FEN)), results[i]);
}

TEST(Perft, Pos4)
{
    std::vector<std::size_t> results {
        1,
        6,
        264,
        9467,
        422333,
        15833292,
        706045033
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft({ .depth=i }, bitboard(POS4_FEN)), results[i]);
}

TEST(Perft, Pos5)
{
    std::vector<std::size_t> results {
        1,
        44,
        1486,
        62379,
        2103487,
        89941194
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft({ .depth=i }, bitboard(POS5_FEN)), results[i]);
}

TEST(Perft, Pos6)
{
    std::vector<std::size_t> results {
        1,
        46,
        2079,
        89890,
        3894594,
        164075551
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft({ .depth=i }, bitboard(POS6_FEN)), results[i]);
}

// Tests performannt perft correctness across a variety of positions.
int main(int argc, char **argv)
{
    set_log2_perft_hash_entries(26); 

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
