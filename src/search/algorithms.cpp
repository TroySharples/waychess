#include "algorithms.hpp"

namespace search
{

search search_from_string(std::string_view str)
{
    if (str == "negamax")
        return negamax;
    if (str == "minimax")
        return minimax;
    if (str == "negamax-prune")
        return negamax_prune;

    throw std::invalid_argument("Invalid search algorithm");
}

std::string search_to_string(search s)
{
    if (s == negamax)
        return "negamax";
    if (s == minimax)
        return "minimax";
    if (s == negamax_prune)
        return "negamax-prune";

    throw std::invalid_argument("Invalid search algorithm");
}

}