#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

namespace
{

void test_move_generation_recursive(const bitboard& bb, const std::uint64_t hash, std::uint8_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0)
        return;

    bitboard bb_copy { bb };
    std::uint64_t hash_copy { hash };

    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };
    for (std::uint8_t i = 0; i < moves; i++)
    {
        const std::uint32_t move = move_buf[i];

        std::uint32_t unmake;
        make_move({ .check_legality = true }, bb_copy, move, unmake, hash_copy);

        test_move_generation_recursive(bb_copy, hash_copy, depth-1, move_buf.subspan(moves));

        unmake_move(bb_copy, unmake, hash_copy);

        EXPECT_EQ(bb, bb_copy);
        EXPECT_EQ(hash, hash_copy);
    }
}

void test_move_generation(const char* fen, std::uint8_t depth)
{
    std::vector<std::uint32_t> move_buf(static_cast<std::size_t>(depth)*MAX_MOVES_PER_POSITION);

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
