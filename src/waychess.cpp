#include <cstdlib>

#include "bit_board.hpp"

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

    return EXIT_SUCCESS;
}