#include "search.hpp"

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

std::vector<search_hash_table_type::entry_type> search_hash_buf;

void set_search_hash_table_bytes(std::size_t bytes)
{
    // We only allocate the log2-floor of the requested entries.
    const std::size_t entries { std::bit_floor(bytes/sizeof(search_hash_table_type::entry_type)) };

    search_hash_buf.resize(entries);
    search_hash_table = search_hash_table_type(search_hash_buf);
}

std::size_t get_search_hash_table_bytes()
{
    return search_hash_table.get_table_bytes();
}

}