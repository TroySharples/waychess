#pragma once

#include "search/search.hpp"

struct puzzle
{
    bitboard bb;
    std::vector<std::uint32_t> moves;

    bool solve(search::search s, std::size_t max_depth, evaluation::evaluation eval, const void* args_eval = nullptr) const;

    friend std::istream& operator>>(std::istream& is, puzzle& v);
};