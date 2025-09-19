#include "statistics.hpp"

#include "position/move.hpp"
#include "utility/logging.hpp"
#include <iomanip>

namespace search
{

void statistics::log_search_info() const
{
    const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    std::ostringstream ss;
    ss << "depth "            << static_cast<int>(depth)
       << " score cp "        << eval
       << " time "            << duration_ms
       << " nodes "           << get_nodes()
       << " nps "             << get_nps()
       << " abnodes "         << abnodes
       << " qnodes "          << qnodes
       << " qdepth "          << qdepth
       << " tt-probes "       << tt_probes
       << " tt-hits "         << tt_hits
       << " tt-hit-rate "     << std::setprecision(2) << get_tt_hit_rate()
       << " aw-misses-low "   << aw_misses_low
       << " aw-misses-high "  << aw_misses_high
       << " aw-misses-total " << get_aw_misses_total()
       << " pv "              << move::to_algebraic_long(pv);
    log(ss.str(), log_level::informational);
}

void statistics::id_update(const statistics& v)
{
    depth = std::max(depth, v.depth);
    qdepth = std::max(qdepth, v.qdepth);

    eval = v.eval;
    pv = v.pv;

    abnodes += v.abnodes;
    qnodes += v.qnodes;

    tt_probes += v.tt_probes;
    tt_hits += v.tt_hits;

    aw_misses_low  += v.aw_misses_low;
    aw_misses_high += v.aw_misses_high;

    time += v.time;
}

}