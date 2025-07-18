#include <cstdlib>

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

static void display_pseudo_legal_moves_from_position(const bitboard& start)
{
    std::cout << "starting-position:" << std::endl;
    std::cout << start << std::endl;

    constexpr make_move_args args { .check_legality = false };

    for (auto move : generate_pseudo_legal_moves(start))
    {
        bitboard next_position = start;
        make_move(args, next_position, move);

        std::cout << "move: " << move::to_algebraic_long(move) << ":" << std::endl;
        std::cout << next_position << std::endl;
    }
}

static void display_strictly_legal_moves_from_position(const bitboard& start)
{
    std::cout << "starting-position:" << std::endl;
    std::cout << start << std::endl;

    constexpr make_move_args args { .check_legality = true };

    for (auto move : generate_pseudo_legal_moves(start))
    {
        bitboard next_position = start;
        if (!make_move(args, next_position, move))
            continue;

        std::cout << "move: " << move::to_algebraic_long(move) << ":" << std::endl;
        std::cout << next_position << std::endl;
    }
}

int main()
{
    std::cout << "move-generation-testing:\n\n";

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
        std::cout << "#####################################################################\n" << std::endl;;
        std::cout << "fen - " << position << "\n" << std::endl;
        display_pseudo_legal_moves_from_position(bitboard(position));
    }

    return EXIT_SUCCESS;
}