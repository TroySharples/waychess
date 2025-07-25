#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

namespace
{

std::size_t perft_recursive(const bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0)
        return 1;

    std::size_t ret {};
    
    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move_buf[i])) [[unlikely]]
            continue;

        ret += perft_recursive(next_position, depth-1, move_buf.subspan(moves));
    }

    return ret;
}

}

std::size_t perft(const bitboard& bb, std::size_t depth)
{
    constexpr std::size_t max_moves_per_position { 218 };
    std::vector<std::uint32_t> move_buf(depth*max_moves_per_position);

    return perft_recursive(bb, depth, move_buf);
}
