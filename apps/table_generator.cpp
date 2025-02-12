#include "hashtable.hpp"
#include "pieces/rook.hpp"

static std::vector<std::vector<std::uint64_t>> buf;

sliding_attack_table create_attack_table_rook()
{
    sliding_attack_table ret;

    for (mailbox i = 0; i < ret.size(); i++)
    {
        const bitboard blocker_squares { get_rook_blocker_squares(i) };
        const auto combinations = get_1s_combinations(blocker_squares);

        // Create our standard C++ map we can then use to create our magic bitboard table. We set all the bits in the key that we
        // don't care about to one (the black-magic bitboards trick). In simple fancy bitboards we would instead mask these off,
        // but doing it this way ends up reducing the size of the hash table we'll eventually have to calculate.
        ret[i].notmask = ~blocker_squares;
        std::map<std::uint64_t, std::uint64_t> map;
        for (const auto comb : combinations)
            map[comb | ret[i].notmask] = get_rook_attacked_squares(i, comb);
        
        // Actually calculate our magic bitboard tables.
        buf.emplace_back();
        ret[i].table = hashtable(buf.back(), map);
    }

    return ret;
}

int main()
{
    const auto attack_table_rook = create_attack_table_rook();
    std::cout << attack_table_rook << std::endl;

    return EXIT_SUCCESS;
}