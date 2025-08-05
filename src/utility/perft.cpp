#include "perft.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"
#include "details/hash_map.hpp"

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

struct perft_value_type
{
    std::uint8_t depth;
    std::size_t  nodes;
};

using perft_hash_table_type = details::hash_table<perft_value_type>;
std::vector<perft_hash_table_type::entry_type> perft_hash_buf;
perft_hash_table_type perft_hash_table;

std::size_t perft_recursive_unmake_hash(bitboard& bb, std::uint64_t& hash, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    // Loop up the value in the hash table and return immediately if we hit.
    auto& entry { perft_hash_table[hash] };
    if (entry.key == hash && entry.value.depth == depth)
        return entry.value.nodes;

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
    entry.key = hash;
    entry.value.depth = depth;
    entry.value.nodes = ret;

    return ret;
}

std::size_t perft_recursive_copy_hash(bitboard& bb, std::uint64_t hash, std::size_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    // Loop up the value in the hash table and return immediately if we hit.
    auto& hash_entry { perft_hash_table[hash] };
    if (hash_entry.key == hash && hash_entry.value.depth == depth)
        return hash_entry.value.nodes;

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
    hash_entry.value.depth = depth;
    hash_entry.value.nodes = ret;

    return ret;
}

}

void set_perft_hash_table_bytes(std::size_t bytes)
{
    // We only allocate the log2-floor of the requested entries.
    const std::size_t entries { std::bit_floor(bytes/sizeof(perft_hash_table_type::entry_type)) };

    perft_hash_buf.resize(entries);
    perft_hash_table = perft_hash_table_type(perft_hash_buf);
}

std::size_t get_perft_hash_table_bytes()
{
    return perft_hash_table.get_table_bytes();
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
    std::vector<std::uint32_t> move_buf(args.depth*MAX_MOVES_PER_POSITION);

    bitboard bb { start };

    // Switch implementations depending whether our hash-table is non-empty.
    if (get_perft_hash_table_bytes() == 0)
    {
        switch (args.strategy)
        {
            case perft_args::copy:   return perft_recursive_copy_no_hash(bb, args.depth, move_buf);
            case perft_args::unmake: return perft_recursive_unmake_no_hash(bb, args.depth, move_buf);
            default: throw std::invalid_argument("Invalid perft-args");
        }
    }
    else
    {
        std::uint64_t hash = zobrist::hash_init(bb);
        switch (args.strategy)
        {
            case perft_args::copy:   return perft_recursive_copy_hash(bb, hash, args.depth, move_buf);
            case perft_args::unmake: return perft_recursive_unmake_hash(bb, hash, args.depth, move_buf);
            default: throw std::invalid_argument("Invalid perft-args");
        }
    }
}
