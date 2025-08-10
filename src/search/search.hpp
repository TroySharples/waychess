#pragma once

// ####################################
// DECLARATION
// ####################################

#include "evaluation/evaluation.hpp"
#include "details/hash_map.hpp"

#include <vector>

namespace search
{

// A general search signature. Functions of this type take a terminal heuristic, and return the
// searched evaluation. The behaviour of the search may be changed with the args pointer (e.g.
// max-depth). This must return an absolute centipawn evaluation (i.e. not relative to the
// playing side).
using search = int (*)(const bitboard& bb, std::size_t max_depth, std::span<std::uint32_t> move_buf, evaluation::evaluation eval, const void* args_eval);

// Entry type for search algorithms that use Zobrist hash-tables. We may add more things to this in the future.
struct __attribute__ ((__packed__)) search_value_type
{
    std::uint8_t depth;
    int eval;

    // A generic meta-field that can be used for various purposes depending on the search.
    std::uint8_t meta;
};

using search_hash_table_type = ::details::hash_table<search_value_type>;
extern std::vector<search_hash_table_type::entry_type> search_hash_buf;
inline search_hash_table_type search_hash_table { search_hash_buf };

// Note only allocates the log2_floor the entries.
void set_search_hash_table_bytes(std::size_t bytes);
std::size_t get_search_hash_table_bytes();

struct recommendation
{
    std::uint32_t move;
    int eval;

    friend constexpr bool operator<(const recommendation& a, const recommendation& b) noexcept { return a.eval < b.eval; };
    friend constexpr bool operator>(const recommendation& a, const recommendation& b) noexcept { return a.eval > b.eval; };
};

recommendation recommend_move(const bitboard& bb, search s, std::size_t max_depth, evaluation::evaluation eval, const void* args_eval = nullptr);

}

// ####################################
// ALGORITHMS
// ####################################

#include "search_negamax.hpp"
#include "search_minimax.hpp"

namespace search
{

// The search algorithms we've implemented so far.
constexpr search negamax       { &search_negamax };
constexpr search minimax       { &search_minimax };
constexpr search negamax_prune { &search_negamax_prune };

search search_from_string(std::string_view str);
std::string search_to_string(search s);

}

// ####################################
// IMPLEMENTATION
// ####################################

namespace search
{

inline recommendation recommend_move(const bitboard& bb, search s, std::size_t max_depth, evaluation::evaluation eval, const void* args_eval)
{
    std::vector<std::uint32_t> move_buf(max_depth*MAX_MOVES_PER_POSITION);
    std::span<std::uint32_t> move_span(move_buf);

    const bool is_black_to_play { bb.is_black_to_play() };
    const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };

    recommendation ret { .move = 0, .eval = is_black_to_play ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min() };

    for (std::size_t i = 0; i < moves; i++)
    {
        bitboard next_position = bb;

        if (!make_move({ .check_legality = true }, next_position, move_buf[i])) [[unlikely]]
            continue;

        if (const int score { s(next_position, max_depth-1, move_span.subspan(moves), eval, args_eval) }; (is_black_to_play ? score < ret.eval : score > ret.eval) )
            ret = { .move = move_buf[i], .eval = score };
    }

    return ret;
}

}