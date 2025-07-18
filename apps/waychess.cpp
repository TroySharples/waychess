#include <cstdlib>
#include <stdexcept>

#include "pieces/king.hpp"
#include "pieces/knight.hpp"
#include "pieces/pieces.hpp"
#include "pieces/rook.hpp"
#include "position/bitboard.hpp"
#include "position/mailbox.hpp"
#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

static void display_pseudo_legal_moves_from_position(const bitboard& start)
{
    std::cout << "starting-position:" << std::endl;
    std::cout << start << std::endl;

    for (auto move : generate_pseudo_legal_moves(start))
    {
        std::cout << "move: " << move::to_algebraic_long(move) << ":" << std::endl;
        bitboard next_position = start;
        make_move(next_position, move);
        std::cout << next_position << std::endl;
    }
}

int main()
{
    // std::cout << "white squares\n";
    // display(std::cout, WHITE_SQUARES);
    // std::cout << '\n';

    // std::cout << "black squares\n";
    // display(std::cout, BLACK_SQUARES);
    // std::cout << '\n';

    // std::cout << "files\n";
    // for (std::size_t i = 0; i < 8; i++)
    // {
    //     display(std::cout, get_bitboard_file(i));
    //     std::cout << '\n';
    // }
    
    // std::cout << "ranks\n";
    // for (std::size_t i = 0; i < 8; i++)
    // {
    //     display(std::cout, get_bitboard_rank(i));
    //     std::cout << '\n';
    // }

    // std::cout << "centre-4 squares\n";
    // display(std::cout, CENTRE_4_SQUARES);
    // std::cout << '\n';

    // std::cout << "centre-16 squares\n";
    // display(std::cout, CENTRE_16_SQUARES);
    // std::cout << '\n';

    // std::cout << "rim squares\n";
    // display(std::cout, RIM_SQUARES);
    // std::cout << '\n';

    // std::cout << "bitscan H-file    - " << bitscan_forward(get_bitboard_file(7)) << '\n';
    // std::cout << '\n';
    // std::cout << "bitscan centre-16 - " << bitscan_forward(CENTRE_16_SQUARES) << '\n';
    // std::cout << '\n';
    // std::cout << "popcount black-squares - " << popcount(BLACK_SQUARES) << '\n';
    // std::cout << '\n';

    // std::cout << "knight-attacks\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_knight_attacked_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "king-attacks\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_king_attacked_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "rook-xrays\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_rook_xrayed_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "centre-combinations\n";
    // const auto centre_combinations = get_1s_combinations(CENTRE_4_SQUARES);
    // for (const auto comb : centre_combinations)
    // {
    //     display(std::cout, comb);
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "rook-attacks\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_rook_attacked_squares_from_mailbox(i, CENTRE_16_SQUARES));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "bishop-xrays\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_bishop_xrayed_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "bishop-blockers\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_bishop_blocker_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "bishop-attacks\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_bishop_attacked_squares_from_mailbox(i, CENTRE_16_SQUARES));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "queen-xrays\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_queen_xrayed_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "queen-blockers\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_queen_blocker_squares_from_mailbox(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "queen-attacks\n";
    // for (std::size_t i = 0; i < 64; i++)
    // {
    //     display(std::cout, get_queen_attacked_squares_from_mailbox(i, CENTRE_16_SQUARES));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

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