#include <cstdlib>

#include "bitboard.hpp"
#include "magic.hpp"
#include "random.hpp"

consteval std::array<std::uint64_t, 1000> generate_keys()
{
    std::uint64_t rand = CONSTEXPR_RANDOM;

    std::array<std::uint64_t, 1000> ret;
    for (std::size_t i = 0; i < ret.size(); i++)
        ret[i] = (rand = constexpr_hash(rand));
    return ret;
}

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
    std::cout << "bitscan centre-16 - " << bitscan_forward(CENTRE_16_SQUARES) << '\n';

    std::cout << "popcount black-squares - " << popcount(BLACK_SQUARES) << '\n';

    constexpr auto keys = generate_keys();
    constexpr magic m { magic_generator(keys) };
    std::cout << "magic - " << m.first << ", " << m.second << '\n';
    for (auto i : keys)
        std::cout << "   hash(" << i << ") - " << hasher(i, m) << '\n';
    

    return EXIT_SUCCESS;
}