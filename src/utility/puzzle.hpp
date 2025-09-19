#pragma once

#include "position/game_state.hpp"

struct solver
{
    struct puzzle;
    bool solve(const puzzle& p, std::size_t depth);

    game_state gs;
};

struct solver::puzzle
{
    bitboard bb;
    std::vector<std::uint64_t> moves;

    friend std::istream& operator>>(std::istream& is, puzzle& v);
};