#include "perft.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

namespace
{

std::size_t perft_recursive_copy(bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    std::size_t ret {};

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        constexpr make_move_args args { .check_legality = true };
        if (!make_move(args, next_position, move_buf[i])) [[unlikely]]
            continue;

        ret += perft_recursive_copy(next_position, depth-1, move_buf.subspan(moves));
    }

    return ret;
}

std::size_t perft_recursive_unmake(bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    std::size_t ret {};

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        constexpr make_move_args args { .check_legality = true };

        std::uint32_t unmake {};
        if (make_move(args, bb, move_buf[i], unmake)) [[likely]]
            ret += perft_recursive_unmake(bb, depth-1, move_buf.subspan(moves));

        unmake_move(bb, unmake);
    }

    return ret;
}

}

perft_args::strategy_type perft_args::strategy_type_from_string(std::string_view str)
{
    if (str == "copy")
        return perft_args::copy;
    if (str == "unmake")
        return perft_args::unmake;

    throw std::invalid_argument("Invalid perft strategy");
}

std::string perft_args::strategy_type_to_string(strategy_type strategy)
{
    switch (strategy)
    {
        case perft_args::copy:   return "copy";
        case perft_args::unmake: return "unmake";
        default: throw std::invalid_argument("Invalid perft strategy");
    }
}

std::size_t perft(const perft_args& args, const bitboard& start)
{
    constexpr std::size_t max_moves_per_position { 218 };
    std::vector<std::uint32_t> move_buf(args.depth*max_moves_per_position);

    bitboard bb { start };

    switch (args.strategy)
    {
        case perft_args::copy:   return perft_recursive_copy(bb, args.depth, move_buf);
        case perft_args::unmake: return perft_recursive_unmake(bb, args.depth, move_buf);
    }
}
