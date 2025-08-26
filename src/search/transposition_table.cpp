#include "transposition_table.hpp"

namespace search
{

details::hash_table<search_value_type> transposition_table;
std::uint8_t transposition_age;

}