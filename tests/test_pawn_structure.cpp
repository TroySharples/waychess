#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

TEST(PawnStructure, StartingPosition)
{
    const bitboard bb(STARTING_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), 0);
}

TEST(PawnStructure, Kiwipete)
{
    const bitboard bb(KIWIPETE_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), 0);
}

TEST(PawnStructure, Pos3)
{
    const bitboard bb(POS3_FEN);

    // All of white's pawns are isolated, and black has a single isolated pawn on f4.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), bb.boards[piece_idx::w_pawn]);
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), (FILE_F & RANK_4));
}

TEST(PawnStructure, Pos4)
{
    const bitboard bb(POS4_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), 0);
}

TEST(PawnStructure, Pos5)
{
    const bitboard bb(POS5_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), 0);
}

TEST(PawnStructure, Pos6)
{
    const bitboard bb(POS6_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), 0);
}

TEST(PawnStructure, Behting)
{
    const bitboard bb(BEHTING_FEN);

    // White and black have single isolated pawns on d2 and c4 respectively.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), (FILE_D & RANK_2));
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), (FILE_C & RANK_4));
}

TEST(PawnStructure, HansSecelle)
{
    const bitboard bb(HANSSECELLE_FEN);

    // White: a7, h5; black a3.
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::w_pawn]), (FILE_A & RANK_7) | (FILE_H & RANK_5));
    ASSERT_EQ(get_isolated(bb.boards[piece_idx::b_pawn]), (FILE_A & RANK_3));
}

// Test correctness of pawn-structure assessment.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
