#pragma once

#include "search/search.hpp"

struct puzzle
{
    bitboard bb;
    std::vector<std::uint32_t> moves;

    bool solve(std::uint8_t max_depth) const;

    friend std::istream& operator>>(std::istream& is, puzzle& v);
};