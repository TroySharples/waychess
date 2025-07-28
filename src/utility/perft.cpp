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

struct perft_hash_entry
{
    std::uint64_t key;
    std::uint8_t  depth;
    std::size_t   nodes;
};

// Simple hash-table only containing the actual key and number of positions for now. This should
// correspond to about a 1GB table.
constexpr std::size_t PERFT_HASH_ENTRIES { 1ULL << 26 };
constexpr std::uint64_t PERFT_HASH_MASK  { 0x3ffffff };
std::array<perft_hash_entry, PERFT_HASH_ENTRIES> perft_hash_table;

std::size_t perft_recursive_hash(bitboard& bb, std::uint64_t hash, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    // Loop up the value in the hash table and return immediately if we hit.
    perft_hash_entry& hash_entry { perft_hash_table[hash & PERFT_HASH_MASK] };
    if (hash_entry.key == hash && hash_entry.depth == depth)
        return hash_entry.nodes;

    std::size_t ret {};

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        constexpr make_move_args args { .check_legality = true };

        std::uint32_t unmake {};
        if (make_move(args, bb, move_buf[i], unmake, hash)) [[likely]]
        ret += perft_recursive_hash(bb, hash, depth-1, move_buf.subspan(moves));

        unmake_move(bb, unmake, hash);
    }

    // Update the hash table with our result - always overriding for now.
    hash_entry.key = hash;
    hash_entry.depth = depth;
    hash_entry.nodes = ret;

    return ret;
}

}

perft_args::strategy_type perft_args::strategy_type_from_string(std::string_view str)
{
    if (str == "copy")
        return perft_args::copy;
    if (str == "unmake")
        return perft_args::unmake;
    if (str == "hash")
        return perft_args::hash;

    throw std::invalid_argument("Invalid perft strategy");
}

std::string perft_args::strategy_type_to_string(strategy_type strategy)
{
    switch (strategy)
    {
        case perft_args::copy:   return "copy";
        case perft_args::unmake: return "unmake";
        case perft_args::hash:   return "hash";
        default: throw std::invalid_argument("Invalid perft strategy");
    }
}

std::size_t perft(const perft_args& args, const bitboard& start)
{
    std::vector<std::uint32_t> move_buf(args.depth*MAX_MOVES_PER_POSITION);

    bitboard bb { start };

    switch (args.strategy)
    {
        case perft_args::copy:   return perft_recursive_copy(bb, args.depth, move_buf);
        case perft_args::unmake: return perft_recursive_unmake(bb, args.depth, move_buf);
        case perft_args::hash:
        {
            std::uint64_t hash = zobrist::hash_init(bb);
            const std::size_t ret { perft_recursive_hash(bb, hash, args.depth, move_buf) };

            return ret;
        }
        default: throw std::invalid_argument("Invalid perft-args");
    }
}
