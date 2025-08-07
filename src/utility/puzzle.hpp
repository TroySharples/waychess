#pragma once

#include "position/search.hpp"

struct puzzle
{
    bitboard bb;
    std::vector<std::uint32_t> moves;

    bool solve(recommendation rec) const ;

    friend std::istream& operator>>(std::istream& is, puzzle& v);
};