#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

#include "utility/perft.hpp"

#include <gtest/gtest.h>

/////////////////////////////////////////////////////////////////////////
// All positions taken from chess programming wiki perft results page. //
// https://www.chessprogramming.org/Perft_Results                      //
/////////////////////////////////////////////////////////////////////////

TEST(Perft, StartingPosition)
{
    constexpr const char* fen { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
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
        EXPECT_EQ(perft(bitboard(fen), i), results[i]);
}

TEST(Perft, Kiwipete)
{
    constexpr const char* fen { "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" };
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
        EXPECT_EQ(perft(bitboard(fen), i), results[i]);
}

TEST(Perft, Pos3)
{
    constexpr const char* fen { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" };
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
        EXPECT_EQ(perft(bitboard(fen), i), results[i]);
}

TEST(Perft, Pos4)
{
    constexpr const char* fen { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1" };
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
        EXPECT_EQ(perft(bitboard(fen), i), results[i]);
}

TEST(Perft, Pos5)
{
    constexpr const char* fen { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8" };
    std::vector<std::size_t> results {
        1,
        44,
        1486,
        62379,
        2103487,
        89941194
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft(bitboard(fen), i), results[i]);
}

TEST(Perft, Pos6)
{
    constexpr const char* fen { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" };
    std::vector<std::size_t> results {
        1,
        46,
        2079,
        89890,
        3894594,
        164075551
    };

    for (std::size_t i = 0; i < results.size(); i++)
        EXPECT_EQ(perft(bitboard(fen), i), results[i]);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
