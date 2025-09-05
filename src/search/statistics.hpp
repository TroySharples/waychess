#pragma once

#include <chrono>
#include <cstddef>

namespace search
{

// Structure for recording metrics from the search. Will extend this later on.
struct statistics
{
    // Number of nodes visited in search.
    std::size_t nodes {};

    // Transposition-table metrics.
    std::size_t tt_probes {};
    std::size_t tt_hits {};

    // Time for current search - handled by the outer loop.
    std::chrono::steady_clock::duration duration;
};

}