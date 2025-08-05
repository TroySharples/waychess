#pragma once

#include "position/bitboard.hpp"

// Note only allocates the log2_floor the entries.
void set_perft_hash_table_bytes(std::size_t bytes);
std::size_t get_perft_hash_table_bytes();

struct perft_args
{
    std::size_t depth;
    enum strategy_type : std::uint8_t { copy_no_hash, unmake_no_hash, copy_hash, unmake_hash } strategy { unmake_hash };

    static strategy_type strategy_type_from_string(std::string_view str);
    static std::string strategy_type_to_string(strategy_type strategy);
};

std::size_t perft(const perft_args& args, const bitboard& start);