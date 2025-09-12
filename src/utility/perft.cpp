#include "perft.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"
#include "details/hash_table.hpp"
#include "position/zobrist_hash.hpp"

namespace
{

std::size_t perft_recursive_unmake_no_hash(bitboard& bb, std::uint8_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    std::size_t ret {};

    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::uint8_t i = 0; i < moves; i++)
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

details::hash_table<perft_value_type> perft_hash_table;

std::size_t perft_recursive_unmake_hash(bitboard& bb, std::uint64_t& hash, std::uint8_t depth, std::span<std::uint32_t> move_buf)
{
    if (depth == 0) [[unlikely]]
        return 1;

    // Loop up the value in the hash table and return immediately if we hit.
    auto& entry { perft_hash_table[hash] };
    if (entry.key == hash && entry.value.depth == depth)
        return entry.value.nodes;

    std::size_t ret {};

    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::uint8_t i = 0; i < moves; i++)
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
}

void set_perft_hash_table_bytes(std::size_t bytes)
{
    perft_hash_table.set_table_bytes(bytes);
}

std::size_t get_perft_hash_table_bytes()
{
    return perft_hash_table.get_table_bytes();
}

std::size_t perft(const bitboard& start, std::uint8_t depth)
{
    std::vector<std::uint32_t> move_buf(static_cast<std::size_t>(depth)*MAX_MOVES_PER_POSITION);

    bitboard bb { start };

    // Switch implementations depending whether our hash-table is non-empty.
    if (get_perft_hash_table_bytes() == 0)
    {
        return perft_recursive_unmake_no_hash(bb, depth, move_buf);
    }
    else
    {
        std::uint64_t hash { zobrist::hash_init(mailbox(bb)) };
        return perft_recursive_unmake_hash(bb, hash, depth, move_buf);
    }
}
