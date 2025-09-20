#pragma once

#include <span>
#include <chrono>
#include <cstddef>

namespace search
{

// Structure for recording metrics from the search. Will extend this later on.
struct statistics
{
    // Maximum PV-search depth.
    std::size_t depth {};
    // Maximum quiescent-search depth.
    std::size_t qdepth {};

    // Final search evaluation and PV.
    int eval;
    std::span<const std::uint32_t> pv;

    // The number of nodes searches for in the main alpha-beta body of the search,
    std::size_t abnodes {};
    // The number of nodes visited in quiescent search.
    std::size_t qnodes {};
    // The number of cut-nodes (fail-high) visited.
    std::size_t cutnodes {};
    // The number of all-nodes (fail-low) visited.
    std::size_t allnodes {};
    // The number of pv-nodes (exact) visited.
    std::size_t pvnodes {};

    std::size_t get_nodes() const noexcept { return abnodes+qnodes; }

    // Statistics on how fail-high moves were found.
    std::size_t fh_hash  {};
    std::size_t fh_null  {};
    std::size_t fh_first {};
    std::size_t fh_later {};
    double get_fh_on_first_rate() const noexcept { return static_cast<double>(fh_first) / static_cast<double>(1+fh_first+fh_later); }

    // Transposition-table metrics.
    std::size_t tt_probes {};
    std::size_t tt_hits   {};
    double get_tt_hit_rate() const noexcept { return static_cast<double>(tt_hits) / static_cast<double>(tt_probes); }

    // The number of misses of our aspiration window (both lower and upper).
    std::size_t aw_misses_low  {};
    std::size_t aw_misses_high {};
    std::size_t get_aw_misses_total() const noexcept { return aw_misses_low+aw_misses_high; }

    // Time for current search - handled by the outer loop.
    std::chrono::steady_clock::duration time;

    std::size_t get_nps() const noexcept { return static_cast<std::size_t>(static_cast<double>(get_nodes()) / std::chrono::duration<double>(time).count()); }

    // UCI-style statistics logging.
    void log_search_info() const;

    // Update the statistics with the another ID search result.
    void id_update(const statistics& v);
};

}