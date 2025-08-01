#include "perft.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"

namespace
{

std::size_t perft_recursive_copy_no_hash(bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf)
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

        ret += perft_recursive_copy_no_hash(next_position, depth-1, move_buf.subspan(moves));
    }

    return ret;
}

std::size_t perft_recursive_unmake_no_hash(bitboard& bb, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    std::size_t ret {};

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        std::uint32_t unmake;
        if (make_move({ .check_legality = true }, bb, move_buf[i], unmake)) [[likely]]
            ret += perft_recursive_unmake_no_hash(bb, depth-1, move_buf.subspan(moves));

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


std::size_t perft_hash_mask {};
std::vector<perft_hash_entry> perft_hash_table;

std::size_t perft_recursive_unmake_hash(bitboard& bb, std::uint64_t& hash, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    // Loop up the value in the hash table and return immediately if we hit.
    perft_hash_entry& hash_entry { perft_hash_table[hash & perft_hash_mask] };
    if (hash_entry.key == hash && hash_entry.depth == depth)
        return hash_entry.nodes;

    std::size_t ret {};

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        std::uint32_t unmake;
        if (make_move({ .check_legality = true }, bb, move_buf[i], unmake, hash)) [[likely]]
            ret += perft_recursive_unmake_hash(bb, hash, depth-1, move_buf.subspan(moves));

        unmake_move(bb, unmake, hash);
    }

    // Update the hash table with our result - always overriding for now.
    hash_entry.key = hash;
    hash_entry.depth = depth;
    hash_entry.nodes = ret;

    return ret;
}

std::size_t perft_recursive_copy_hash(bitboard& bb, std::uint64_t hash, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    // Loop up the value in the hash table and return immediately if we hit.
    perft_hash_entry& hash_entry { perft_hash_table[hash & perft_hash_mask] };
    if (hash_entry.key == hash && hash_entry.depth == depth)
        return hash_entry.nodes;

    std::size_t ret {};

    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard bitboard_copy { bb };
        std::uint64_t hash_copy { hash };

        std::uint32_t unmake;
        if (make_move({ .check_legality = true }, bitboard_copy, move_buf[i], unmake, hash_copy)) [[likely]]
            ret += perft_recursive_copy_hash(bitboard_copy, hash_copy, depth-1, move_buf.subspan(moves));
    }

    // Update the hash table with our result - always overriding for now.
    hash_entry.key = hash;
    hash_entry.depth = depth;
    hash_entry.nodes = ret;

    return ret;
}

}

void set_log2_perft_hash_entries(std::size_t log2_entries)
{
    const std::size_t entries { 1ULL << log2_entries };

    perft_hash_mask = entries - 1;

    perft_hash_table.resize(entries);
    perft_hash_table.shrink_to_fit();
}

perft_args::strategy_type perft_args::strategy_type_from_string(std::string_view str)
{
    if (str == "copy-no-hash")
        return perft_args::copy_no_hash;
    if (str == "unmake-no-hash")
        return perft_args::unmake_no_hash;
    if (str == "copy-hash")
        return perft_args::copy_hash;
    if (str == "unmake-hash")
        return perft_args::unmake_hash;

    throw std::invalid_argument("Invalid perft strategy");
}

std::string perft_args::strategy_type_to_string(strategy_type strategy)
{
    switch (strategy)
    {
        case perft_args::copy_no_hash:   return "copy-no-hash";
        case perft_args::unmake_no_hash: return "unmake-no-hash";
        case perft_args::copy_hash:      return "copy-hash";
        case perft_args::unmake_hash:    return "unmake-hash";
        default: throw std::invalid_argument("Invalid perft strategy");
    }
}

std::size_t perft(const perft_args& args, const bitboard& start)
{
    std::vector<std::uint32_t> move_buf(args.depth*MAX_MOVES_PER_POSITION);

    bitboard bb { start };

    switch (args.strategy)
    {
        case perft_args::copy_no_hash:   return perft_recursive_copy_no_hash(bb, args.depth, move_buf);
        case perft_args::unmake_no_hash: return perft_recursive_unmake_no_hash(bb, args.depth, move_buf);
        case perft_args::copy_hash:
        {
            std::uint64_t hash = zobrist::hash_init(bb);
            return perft_recursive_copy_hash(bb, hash, args.depth, move_buf);
        }
        case perft_args::unmake_hash:
        {
            std::uint64_t hash = zobrist::hash_init(bb);
            return perft_recursive_unmake_hash(bb, hash, args.depth, move_buf);
        }
        default: throw std::invalid_argument("Invalid perft-args");
    }
}
