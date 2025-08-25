#pragma once

#include "search.hpp"

#include "evaluation/evaluation.hpp"

#include "position/generate_moves.hpp"
#include "position/make_move.hpp"

#include <limits>

namespace search
{

namespace details
{

inline int search_negamax(const bitboard& bb, std::uint8_t depth, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
{
    // It's a draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return 0;

    if (depth == 0)
        return colour*eval(bb, args);

    int ret { -std::numeric_limits<int>::max() };

    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::uint8_t i = 0; i < moves; i++)
    {
        if (stop_search) [[unlikely]]
            return ret;

        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        ret = std::max(ret, -search_negamax(next_position, depth-1, -colour, eval, move_buf.subspan(moves), args));
    }

    // Handle the rare case of stalemate where there are no legal moves in the position but we aren't in
    // check.
    if (ret == -std::numeric_limits<int>::max() && !is_in_check(bb, colour == -1)) [[unlikely]]
        ret = 0;

    return ret;
}

inline int search_negamax_prune_no_hash(const bitboard& bb, std::uint8_t depth, int a, int b, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
{
    // It's a draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return 0;

    if (depth == 0)
        return colour*eval(bb, args);

    int ret { -std::numeric_limits<int>::max() };

    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::uint8_t i = 0; i < moves; i++)
    {
        if (stop_search) [[unlikely]]
            return ret;

        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        ret = std::max(ret, -search_negamax_prune_no_hash(next_position, depth-1, -b, -a, -colour, eval, move_buf.subspan(moves), args));

        a = std::max(a, ret);
        if (a >= b)
            break;
    }

    // Handle the rare case of stalemate where there are no legal moves in the position but we aren't in
    // check.
    if (ret == -std::numeric_limits<int>::max() && !is_in_check(bb, colour == -1)) [[unlikely]]
        ret = 0;

    return ret;
}

inline int search_negamax_prune_hash(const bitboard& bb, std::uint64_t& hash, std::uint8_t depth, int a, int b, int colour, evaluation::evaluation eval, std::span<std::uint32_t> move_buf, const void* args) noexcept
{
    // It's a draw if our half-move clock goes too high.
    if (bb.ply_50m >= 100) [[unlikely]]
        return 0;

    if (depth == 0)
        return colour*eval(bb, args);

    // Loop up the value in the hash table and return immediately if we hit.
    auto& entry { search_hash_table[hash] };
    if (entry.key == hash && entry.value.depth <= depth)
        return entry.value.eval;

    int ret { -std::numeric_limits<int>::max() };

    const std::uint8_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    for (std::uint8_t i = 0; i < moves; i++)
    {
        if (stop_search) [[unlikely]]
            return ret;

        bitboard bitboard_copy { bb };
        std::uint64_t hash_copy { hash };

        std::uint32_t unmake;
        if (!make_move({ .check_legality = true }, bitboard_copy, move_buf[i], unmake, hash_copy)) [[unlikely]]
            continue;

        ret = std::max(ret, -search_negamax_prune_hash(bitboard_copy, hash_copy, depth-1, -b, -a, -colour, eval, move_buf.subspan(moves), args));

        a = std::max(a, ret);
        if (a >= b)
            break;
    }

    // Handle the rare case of stalemate where there are no legal moves in the position but we aren't in
    // check.
    if (ret == -std::numeric_limits<int>::max() && !is_in_check(bb, colour == -1)) [[unlikely]]
        ret = 0;

    // Update the hash table with our result - always overriding for now.
    entry.key = hash;
    entry.value.depth = depth;
    entry.value.eval  = ret;

    return ret;
}

}

inline int search_negamax(const bitboard& bb, std::uint8_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval)
{
    const int colour = (bb.is_black_to_play() ? -1 : 1);

    return colour*details::search_negamax(bb, depth, colour, eval, move_buf, args_eval);
}

inline int search_negamax_prune(const bitboard& bb, std::uint8_t depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval)
{
    const int colour = (bb.is_black_to_play() ? -1 : 1);

    if (get_search_hash_table_bytes() == 0)
    {
        return colour*details::search_negamax_prune_no_hash(bb, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, eval, move_buf, args_eval);
    }
    else
    {
        std::uint64_t hash = zobrist::hash_init(bb);
        return colour*details::search_negamax_prune_hash(bb, hash, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), colour, eval, move_buf, args_eval);
    }
}

}