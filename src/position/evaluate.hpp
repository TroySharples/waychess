#pragma once

#include "bitboard.hpp"

// A generic terminal evaluation signature.
using evaluation = std::int16_t (*)(const bitboard& bb);

// Our terminal evaluation heuristic - may add more of these in the future.
std::int16_t evaluate_terminal(const bitboard& bb) noexcept;

#include "details/evaluate.hpp"