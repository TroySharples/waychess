#include <cstdlib>

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

static void display_pseudo_legal_moves_from_position(const bitboard& start)
{
    std::cout << "starting-position:" << std::endl;
    start.display_unicode_board(std::cout) << std::endl;

    constexpr make_move_args args { .check_legality = false };

    std::array<std::uint32_t, 256> move_buf;
    const std::size_t moves { generate_pseudo_legal_moves(start, move_buf) };
    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = start;
        make_move(args, next_position, move_buf[i]);

        std::cout << "move: " << move::to_algebraic_long(move_buf[i]) << ":" << std::endl;
        next_position.display_unicode_board(std::cout) << std::endl;
    }
}

static void display_strictly_legal_moves_from_position(const bitboard& start)
{
    std::cout << "starting-position:" << std::endl;
    start.display_unicode_board(std::cout) << std::endl;

    constexpr make_move_args args { .check_legality = true };

    std::array<std::uint32_t, 256> move_buf;
    const std::size_t moves { generate_pseudo_legal_moves(start, move_buf) };
    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = start;
        if (!make_move(args, next_position, move_buf[i]))
            continue;

        std::cout << "move: " << move::to_algebraic_long(move_buf[i]) << ":" << std::endl;
        next_position.display_unicode_board(std::cout);
        std::cout << next_position.get_fen_string() << '\n' << std::endl;
    }
}

int main()
{
    {
        std::cout << "pseudo-legal move-generation:\n\n";

        std::vector<std::string> positions {
            // Starting position.
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            // Complicated middlegame.
            "r3k2r/pp1n2pp/2p2q2/b2p1n2/BP1Pp3/P1N2P2/2PB2PP/R2Q1RK1 w kq - 0 13",
            // En-passent.
            "rnbqkbnr/1pppp1pp/p7/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 1",
            // Promotion.
            "1r2k3/2P5/8/8/8/8/8/5K2 w - - 0 1",
            // Castling.
            "rnbqkbnr/pppppppp/8/8/8/8/P6P/R3K2R w KQkq - 0 1"
        };

        for (const auto& position : positions)
        {
            std::cout << "#####################################################################\n" << std::endl;
            std::cout << "fen - " << position << "\n" << std::endl;
            display_pseudo_legal_moves_from_position(bitboard(position));
        }
    }

    {
        std::cout << "strictly-legal move-generation:\n\n";

        std::vector<std::string> positions {
            // Starting position.
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            // Checks everywhere.
            "8/2K5/R7/4k3/6B1/2N5/8/8 b - - 0 1",
            // Castling from check.
            "r3k2r/p6p/8/3K3B/8/8/8/8 b kq - 0 1",
            // Castling into check.
            "r3k2r/p6p/8/3K4/6B1/8/8/8 b kq - 0 1",
            // Castling through check king-side.
            "r3k2r/p6p/8/2BK4/8/8/8/8 b kq - 0 1",
            // Castling through check queen-side.
            "r3k2r/p6p/5B2/3K4/8/8/8/8 b kq - 0 1",
            // Talkchess gotcha.
            "r3k2r/Pppp1ppp/1b3nbN/nPB5/B1P1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 1 1",
            // Kiwipete gotcha.
            "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/2KR3R b kq - 1 1"
        };

        for (const auto& position : positions)
        {
            std::cout << "#####################################################################\n" << std::endl;
            std::cout << "fen - " << position << "\n" << std::endl;
            display_strictly_legal_moves_from_position(bitboard(position));
        }
    }

    return EXIT_SUCCESS;
}