#include <cstdlib>

#include "bitboard.hpp"
#include "hashmap.hpp"
#include "pieces.hpp"

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
        display(std::cout, get_file_squares(i));
        std::cout << '\n';
    }
    
    std::cout << "ranks\n";
    for (std::size_t i = 0; i < 8; i++)
    {
        display(std::cout, get_rank_squares(i));
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

    std::cout << "bitscan H-file    - " << bitscan_forward(get_file_squares(7)) << '\n';
    std::cout << '\n';
    std::cout << "bitscan centre-16 - " << bitscan_forward(CENTRE_16_SQUARES) << '\n';
    std::cout << '\n';
    std::cout << "popcount black-squares - " << popcount(BLACK_SQUARES) << '\n';
    std::cout << '\n';

    std::cout << "knight-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, attack_table_knight[i]);
        std::cout << '\n';
    }
    std::cout << '\n';

    std::cout << "king-attacks\n";
    for (std::size_t i = 0; i < 64; i++)
    {
        display(std::cout, attack_table_king[i]);
        std::cout << '\n';
    }
    std::cout << '\n';

    return EXIT_SUCCESS;
}