#include "evaluation/evaluate.hpp"
#include "pieces/pieces.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

namespace
{

void test_move_generation_recursive(const bitboard& bb, const std::uint64_t hash, const evaluation::piece_square_eval eval, std::size_t depth)
{
    if (depth == 0)
        return;

    bitboard bb_copy           { bb };
    std::uint64_t hash_copy    { hash };
    evaluation::piece_square_eval eval_copy { eval };

    // Our buffer of moves to loop over will also include a null-moves.
    constexpr std::size_t max_moves_in_position_including_null_move { MAX_MOVES_PER_POSITION+1 };
    std::vector<std::uint32_t> move_buf(depth*max_moves_in_position_including_null_move);

    // Generate our move-list and add a null-move on the end to test.
    std::size_t moves { generate_pseudo_legal_moves(bb, std::span<std::uint32_t>(move_buf)) };
    move_buf[moves++] = move::NULL_MOVE;

    for (std::size_t i = 0; i < moves; i++)
    {
        const std::uint32_t make = move_buf[i];

        std::uint32_t unmake;
        if (make_move({ .check_legality = true }, bb_copy, make, unmake, hash_copy, eval_copy))
            test_move_generation_recursive(bb_copy, hash_copy, eval_copy, depth-1);

        unmake_move(bb_copy, make, unmake, hash_copy, eval_copy);

        ASSERT_TRUE(bb_copy.is_consistent());
        EXPECT_EQ(bb,   bb_copy);
        EXPECT_EQ(hash, hash_copy);
        EXPECT_EQ(eval, eval_copy);
        EXPECT_EQ(eval, evaluation::piece_square_eval(bb_copy));
    }
}

void test_move_generation(const char* fen, std::size_t depth)
{
    bitboard bb { fen };
    std::uint64_t hash = zobrist::hash_init(bb);
    evaluation::piece_square_eval eval(bb);
    test_move_generation_recursive(bb, hash, eval, depth);
}

}

TEST(MoveGeneration, StartingPosition)
{
    test_move_generation(STARTING_FEN, 6);
}

TEST(MoveGeneration, Pos3)
{
    test_move_generation(POS3_FEN, 7);
}

TEST(MoveGeneration, Pos4)
{
    test_move_generation(POS4_FEN, 5);
}

TEST(MoveGeneration, Pos5)
{
    test_move_generation(POS5_FEN, 4);
}

TEST(MoveGeneration, Pos6)
{
    test_move_generation(POS6_FEN, 4);
}

// Tests the correctness of make / unmake / zobrist hashing in a non-performant
// way across a variety of positions.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
