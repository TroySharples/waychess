#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

namespace
{

void test_move_generation_recursive(bitboard& bb, std::uint64_t& hash, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0)
        return;

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    const bitboard bb_orig { bb };
    const std::uint64_t hash_orig { hash };
    for (std::size_t i = 0; i < moves; i++)
    {
        constexpr make_move_args args { .check_legality = false };

        std::uint32_t unmake {};
        make_move(args, bb, move_buf[i], unmake, hash);

        test_move_generation_recursive(bb, hash, depth-1, move_buf.subspan(moves));

        unmake_move(bb, unmake, hash);

        EXPECT_EQ(bb, bb_orig);
        EXPECT_EQ(hash, hash_orig);
    }
}

void test_move_generation(const char* fen, std::size_t depth)
{
    std::vector<std::uint32_t> move_buf(depth*MAX_MOVES_PER_POSITION);

    bitboard bb { fen };
    std::uint64_t hash = zobrist::hash_init(bb);
    test_move_generation_recursive(bb, hash, depth, move_buf);
}

}

TEST(MoveGeneration, StartingPosition)
{
    test_move_generation(STARTING_FEN, 7);
}

TEST(MoveGeneration, Kiwipete)
{
    test_move_generation(KIWIPETE_FEN, 6);
}

TEST(MoveGeneration, Pos3)
{
    test_move_generation(POS3_FEN, 8);
}

TEST(MoveGeneration, Pos4)
{
    test_move_generation(POS4_FEN, 6);
}

TEST(MoveGeneration, Pos5)
{
    test_move_generation(POS5_FEN, 5);
}

TEST(MoveGeneration, Pos6)
{
    test_move_generation(POS6_FEN, 5);
}

// Tests the correctness of make / unmake / zobrist hashing in a non-performant
// way across a variety of positions.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
