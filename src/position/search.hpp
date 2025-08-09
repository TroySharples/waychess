#pragma once

#include "evaluate.hpp"

int evaluate_minimax(const bitboard& bb, std::size_t depth, evaluation eval) noexcept;

// Undefined behaviour if depth is 0;
std::pair<std::uint32_t, int> best_move_minimax(const bitboard& bb, std::size_t depth, evaluation eval) noexcept;

#include "details/search.hpp"