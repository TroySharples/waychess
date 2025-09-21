#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

TEST(PawnStructure, StartingPosition)
{
    const bitboard bb(STARTING_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), 0);

    // No doubled-pawns for either side.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), 0);

    // No blocked-pawns for either side.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), 0);
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), 0);
}

TEST(PawnStructure, Kiwipete)
{
    const bitboard bb(KIWIPETE_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), 0);

    // No doubled-pawns for either side.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), 0);

    // Lots of blocked pawns for both sides.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), (FILE_C & RANK_2) | (FILE_E & RANK_4) | (FILE_F & RANK_2) | (FILE_H & RANK_2));
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), (FILE_A & RANK_7) | (FILE_E & RANK_6) | (FILE_F & RANK_7) | (FILE_H & RANK_3));
}

TEST(PawnStructure, Pos3)
{
    const bitboard bb(POS3_FEN);

    // All of white's pawns are isolated, and black has a single isolated pawn on f4.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), bb.boards[piece_idx::w_pawn]);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), (FILE_F & RANK_4));

    // No doubled-pawns for either side.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), 0);

    // No blocked-pawns for either side.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), 0);
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), 0);
}

TEST(PawnStructure, Pos4)
{
    const bitboard bb(POS4_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), 0);

    // White has doubled a-pawns and black has doubled b-pawns.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), (FILE_A & RANK_2) | (FILE_A & RANK_7));
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), (FILE_B & RANK_2) | (FILE_B & RANK_7));

    // Lots of blocked pawns for both sides.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), (FILE_A & RANK_2) | (FILE_A & RANK_7) | (FILE_B & RANK_5));
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), (FILE_B & RANK_7) | (FILE_F & RANK_7) | (FILE_G & RANK_7) | (FILE_H & RANK_7));
}

TEST(PawnStructure, Pos5)
{
    const bitboard bb(POS5_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), 0);

    // No doubled-pawns for either side.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), 0);

    // Single blocked pawn for white on d7.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), FILE_D & RANK_7);
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), 0);
}

TEST(PawnStructure, Pos6)
{
    const bitboard bb(POS6_FEN);

    // No isolated-pawns for either side.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), 0);

    // No doubled-pawns for either side.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), 0);

    // Lots of blocked pawns for both sides.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), (FILE_C & RANK_2) | (FILE_E & RANK_4) | (FILE_F & RANK_2));
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), (FILE_C & RANK_7) | (FILE_E & RANK_5) | (FILE_F & RANK_7) | (FILE_F & RANK_7));
}

TEST(PawnStructure, Behting)
{
    const bitboard bb(BEHTING_FEN);

    // White and black have single isolated pawns on d2 and c4 respectively.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), FILE_D & RANK_2);
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), FILE_C & RANK_4);

    // Black has doubled h-pawns.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), (FILE_H & RANK_4) | (FILE_H & RANK_6));

    // Single blocked pawn for black on h6.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), 0);
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), FILE_H & RANK_6);
}

TEST(PawnStructure, HansSecelle)
{
    const bitboard bb(HANSSECELLE_FEN);

    // White: a7, h5; black a3.
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::w_pawn]), (FILE_A & RANK_7) | (FILE_H & RANK_5));
    ASSERT_EQ(get_pawn_isolated(bb.boards[piece_idx::b_pawn]), FILE_A & RANK_3);

    // No doubled-pawns for either side.
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::w_pawn]), 0);
    ASSERT_EQ(get_pawn_doubled(bb.boards[piece_idx::b_pawn]), 0);

    // No blocked-pawns for either side.
    const std::uint64_t occ { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };
    ASSERT_EQ(get_white_pawn_blocked(bb.boards[piece_idx::w_pawn], occ), 0);
    ASSERT_EQ(get_black_pawn_blocked(bb.boards[piece_idx::b_pawn], occ), 0);
}

// Test correctness of pawn-structure assessment.
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
