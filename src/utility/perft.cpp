#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

std::size_t perft(const bitboard& bb, std::size_t depth)
{
    if (depth == 0)
        return 1;

    
    std::size_t ret {};
    for (auto move : generate_pseudo_legal_moves(bb))
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move))
            continue;

        ret += perft(next_position, depth-1);
    }

    return ret;
}
