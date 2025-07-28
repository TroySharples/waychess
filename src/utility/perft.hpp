#pragma once

#include "position/bitboard.hpp"

struct perft_args
{
    std::size_t depth;
    enum strategy_type{ copy, unmake, hash } strategy { copy };

    static strategy_type strategy_type_from_string(std::string_view str);
    static std::string strategy_type_to_string(strategy_type strategy);
};

std::size_t perft(const perft_args& args, const bitboard& start);