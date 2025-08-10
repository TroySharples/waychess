#pragma once

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