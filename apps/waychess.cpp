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

int main()
{
    std::cout << "white squares\n";
    display(std::cout, WHITE_SQUARES);
    std::cout << '\n';

    std::cout << "black squares\n";
    display(std::cout, BLACK_SQUARES);
    std::cout << '\n';

    std::cout << "files\n";
    for (std::size_t i = 0; i < 8; i++)
    {
        display(std::cout, get_bitboard_file(i));
        std::cout << '\n';
    }
    
    std::cout << "ranks\n";
    for (std::size_t i = 0; i < 8; i++)
    {
        display(std::cout, get_bitboard_rank(i));
        std::cout << '\n';
    }

    std::cout << "centre-4 squares\n";
    display(std::cout, CENTRE_4_SQUARES);
    std::cout << '\n';

    std::cout << "centre-16 squares\n";
    display(std::cout, CENTRE_16_SQUARES);
    std::cout << '\n';

    std::cout << "rim squares\n";
    display(std::cout, RIM_SQUARES);
    std::cout << '\n';

    std::cout << "bitscan H-file    - " << bitscan_forward(get_bitboard_file(7)) << '\n';
    std::cout << '\n';
    std::cout << "bitscan centre-16 - " << bitscan_forward(CENTRE_16_SQUARES) << '\n';
    std::cout << '\n';
    std::cout << "popcount black-squares - " << popcount(BLACK_SQUARES) << '\n';
    std::cout << '\n';

    std::cout << "knight-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_knight_attacked_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "king-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_king_attacked_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "rook-xrays\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_rook_xrayed_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "centre-combinations\n";
    const auto centre_combinations = get_1s_combinations(CENTRE_4_SQUARES);
    for (const auto comb : centre_combinations)
    {
        display(std::cout, comb);
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "rook-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_rook_attacked_squares_from_mailbox(i, CENTRE_16_SQUARES));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "bishop-xrays\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_bishop_xrayed_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "bishop-blockers\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_bishop_blocker_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "bishop-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_bishop_attacked_squares_from_mailbox(i, CENTRE_16_SQUARES));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "queen-xrays\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_queen_xrayed_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "queen-blockers\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_queen_blocker_squares_from_mailbox(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "queen-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, get_queen_attacked_squares_from_mailbox(i, CENTRE_16_SQUARES));
        std::cout << '\n';
    }
    std::cout << '\n';

    constexpr const char* STARTING_FEN { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
    std::cout << "starting-position\n";
    std::cout << bitboard(STARTING_FEN);
    std::cout << '\n';

    constexpr const char* COMPLICATED_FEN { "r3k2r/pp1n2pp/2p2q2/b2p1n2/BP1Pp3/P1N2P2/2PB2PP/R2Q1RK1 w kq b3 0 13" };
    std::cout << "complicated-position\n";
    std::cout << bitboard(COMPLICATED_FEN);
    if (const auto fen = bitboard(COMPLICATED_FEN).get_fen_string(); fen != COMPLICATED_FEN)
        throw std::logic_error("Invalid fen serialisation - " + fen);
    std::cout << '\n';

    std::cout << "First moves\n";
    {
        bitboard start(COMPLICATED_FEN);
        for (auto move : generate_pseudo_legal_moves(start))
        {
            bitboard next_position = start;
            make_move(next_position, move);
            std::cout << next_position << '\n';
        }
    }

    return EXIT_SUCCESS;
}