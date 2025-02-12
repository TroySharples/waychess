#include <cstdlib>

#include "board.hpp"
#include "pieces/pieces.hpp"
#include "pieces/rook.hpp"

int main()
{
    std::cout << "white squares\n";
    display(std::cout, WHITE_SQUARES);
    std::cout << '\n';

    std::cout << "black squares\n";
    display(std::cout, BLACK_SQUARES);
    std::cout << '\n';

    std::cout << "files\n";
    for (mailbox i = 0; i < 8; i++)
    {
        display(std::cout, get_bitboard_file(i));
        std::cout << '\n';
    }
    
    std::cout << "ranks\n";
    for (mailbox i = 0; i < 8; i++)
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
    for (mailbox i = 0; i < 64; i++)
    {
        display(std::cout, attack_table_knight[i]);
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "king-attacks\n";
    for (mailbox i = 0; i < 64; i++)
    {
        display(std::cout, attack_table_king[i]);
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "rook-xrays\n";
    for (mailbox i = 0; i < 64; i++)
    {
        display(std::cout, get_rook_xrayed_squares(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "ne-diagonal\n";
    for (mailbox i = 0; i < 64; i++)
    {
        display(std::cout, get_bitboard_mailbox_ne_diagonal(i));
        std::cout << '\n';
    }
    std::cout << '\n';

    // std::cout << "nw-diagonal\n";
    // for (mailbox i = 0; i < 16; i++)
    // {
    //     display(std::cout, get_bitboard_mailbox_nw_diagonal(i));
    //     std::cout << '\n';
    // }
    // std::cout << '\n';

    std::cout << "centre-combinations\n";
    const auto centre_combinations = get_1s_combinations(CENTRE_4_SQUARES);
    for (const auto comb : centre_combinations)
    {
        display(std::cout, comb);
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "rook-attacks\n";
    for (mailbox i = 0; i < 64; i++)
    {
        display(std::cout, get_rook_attacked_squares(i, CENTRE_16_SQUARES));
        std::cout << '\n';
    }
    std::cout << '\n';

    return EXIT_SUCCESS;
}