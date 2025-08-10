#include "search.hpp"

namespace search
{

bool stop_search { false };

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