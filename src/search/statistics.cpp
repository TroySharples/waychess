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
    ss << "depth "                 << static_cast<int>(depth)
       << " score cp "             << eval
       << " time "                 << duration_ms
       << " nodes "                << get_nodes()
       << " nps "                  << get_nps()
       << " abnodes "              << abnodes
       << " qnodes "               << qnodes
       << " cutnodes "             << cutnodes
       << " allnodes "             << allnodes
       << " pvnodes "              << pvnodes
       << " moves-all "            << moves_all
       << " moves-null "           << moves_null
       << " moves-illegal "        << moves_illegal
       << " moves-pvs "            << moves_pvs
       << " moves-lmr "            << moves_lmr
       << " moves-cut"             << moves_cut
       << " moves-improve"         << moves_improve
       << " moves-null-rate "      << get_moves_null_rate()
       << " moves-illegal-rate "   << get_moves_illegal_rate()
       << " moves-pvs-rate "       << get_moves_pvs_rate()
       << " moves-lmr-rate "       << get_moves_lmr_rate()
       << " moves-cut-rate "       << get_moves_cut_rate()
       << " moves-improve-rate "   << get_moves_improve_rate()
       << " fh-hash "              << fh_hash
       << " fh-null "              << fh_null
       << " fh-first "             << fh_first
       << " fh-later "             << fh_later
       << " fh-on-null-rate "      << get_fh_on_null_rate()
       << " fh-on-first-rate "     << get_fh_on_first_rate()
       << " qdepth "               << qdepth
       << " tt-probes "            << tt_probes
       << " tt-hits "              << tt_hits
       << " tt-hit-rate "          << std::setprecision(2) << get_tt_hit_rate()
       << " aw-misses-low "        << aw_misses_low
       << " aw-misses-high "       << aw_misses_high
       << " aw-misses-total "      << get_aw_misses_total()
       << " pvs-researches "       << pvs_researches
       << " pvs-research-rate "    << get_pvs_research_rate()
       << " lmr-researches "       << lmr_researches
       << " lmr-research-rate "    << get_lmr_research_rate()
       << " pv "                   << move::to_algebraic_long(pv);
    log(ss.str(), log_level::informational);
}

void statistics::id_update(const statistics& v)
{
    depth  = std::max(depth,  v.depth);
    qdepth = std::max(qdepth, v.qdepth);

    eval = v.eval;
    pv   = v.pv;

    abnodes  += v.abnodes;
    qnodes   += v.qnodes;
    cutnodes += v.cutnodes;
    allnodes += v.allnodes;
    pvnodes  += v.pvnodes;

    moves_all     += v.moves_all;
    moves_null    += v.moves_null;
    moves_illegal += v.moves_illegal;
    moves_pvs     += v.moves_pvs;
    moves_lmr     += v.moves_lmr;
    moves_cut     += v.moves_cut;
    moves_improve += v.moves_improve;

    fh_hash  += v.fh_hash;
    fh_null  += v.fh_null;
    fh_first += v.fh_first;
    fh_later += v.fh_later;

    tt_probes += v.tt_probes;
    tt_hits   += v.tt_hits;

    aw_misses_low  += v.aw_misses_low;
    aw_misses_high += v.aw_misses_high;

    pvs_researches += v.pvs_researches;

    lmr_researches += v.lmr_researches;

    time += v.time;
}

}