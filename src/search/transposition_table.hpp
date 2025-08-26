#pragma once

#include "details/hash_table.hpp"

namespace search
{

// The value type in our hash table for search - we may expand this in the future.
struct __attribute__ ((__packed__)) search_value_type
{
    std::uint8_t depth;
    int eval;

    // A generic meta-field that can be used for various purposes depending on the search.
    std::uint8_t meta;
};

// We have one global transposition table.
extern ::details::hash_table<search_value_type> transposition_table;

}