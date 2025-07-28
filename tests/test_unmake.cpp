#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

#include "test_positions.hpp"

#include <gtest/gtest.h>

namespace
{

void test_unmake_recursive(bitboard& bb, std::uint64_t& hash, std::size_t depth, std::span<std::uint32_t> move_buf)
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

        test_unmake_recursive(bb, hash, depth-1, move_buf.subspan(moves));

        unmake_move(bb, unmake, hash);

        EXPECT_EQ(bb, bb_orig);
        EXPECT_EQ(hash, hash_orig);
    }
}

void test_unmake(const char* fen, std::size_t depth)
{
    std::vector<std::uint32_t> move_buf(depth*MAX_MOVES_PER_POSITION);

    bitboard bb { fen };
    std::uint64_t hash = zobrist::hash_init(bb);
    test_unmake_recursive(bb, hash, depth, move_buf);
}

}

TEST(Unmake, StartingPosition)
{
    test_unmake(STARTING_FEN, 7);
}

TEST(Unmake, Kiwipete)
{
    test_unmake(KIWIPETE_FEN, 6);
}

TEST(Unmake, Pos3)
{
    test_unmake(POS3_FEN, 8);
}

TEST(Unmake, Pos4)
{
    test_unmake(POS4_FEN, 6);
}

TEST(Unmake, Pos5)
{
    test_unmake(POS5_FEN, 5);
}

TEST(Unmake, Pos6)
{
    test_unmake(POS6_FEN, 5);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
