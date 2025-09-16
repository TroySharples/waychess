#include "config.hpp"
#include "search/search.hpp"
#include "search/statistics.hpp"
#include "utility/logging.hpp"

#include <cstring>
#include <unistd.h>
#include <iomanip>

static std::ostream& print_usage(const char* argv0, std::ostream& os)
{
    return os << "Usage: " << argv0 << " <options>\n"
              << "    Options:\n"
              << "         -h                   -> Print this help menu.\n"
              << "         -f [fen]             -> The FEN string for the starting position. Optional, defaults to starting position.\n"
              << "         -d [depth]           -> The evaluation depth. Optional, default 1.\n"
              << "         -k [hash-table size] -> The size of the hash-table (in MiB) if used. Optional, default 1000.\n";
}

int main(int argc, char** argv)
{
    // Default arguments.
    bool help                         { false };
    std::string fen                   { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
    std::size_t depth                 { 1 };
    std::size_t hash_table_size_bytes { 1000000000ULL };

    // Parse options.
    for (int c; (c = getopt(argc, argv, "hf:d:s:k:")) != -1; )
    {
        switch (c)
        {
            // Help.
            case 'h':
            {
                help = true;
                break;
            }
            // FEN.
            case 'f':
            {
                fen = optarg;
                break;
            }
            // Depth.
            case 'd':
            {
                depth = std::stoull(optarg);
                break;
            }
            // Hash size.
            case 'k':
            {
                hash_table_size_bytes = std::stoull(optarg)*1000000;
                break;
            }
            // Unknown
            case '?':
            {
                if (optopt == 'f' || optopt == 'd')
                {
                    std::cerr << "Option requires argument.\n";
                    return EXIT_FAILURE;
                }
                break;
            }
            default:
                std::cerr << "Could not parse commandline arguments.\n";
                print_usage(argv[0], std::cerr);
                return EXIT_FAILURE;
        }
    }

    // Just print usage menu and return if we asked for help.
    if (help)
    {
        print_usage(argv[0], std::cout);
        return EXIT_SUCCESS;
    }

    // Setup the logger.
    set_log_method(log_method::cerr);

    game_state gs;
    gs.reset();
    gs.load(bitboard(fen));

    // Init hash table, so we can read-back the actual allocated memory (instead of the requested amount) when
    // printing out the telemetry below.
    gs.tt.set_table_bytes(hash_table_size_bytes);

    search::statistics stats {};
    const search::recommendation rec { search::recommend_move(gs, stats, depth) };

    // Start printing the JSON file in one go. We might clean this up later by having a proper JSON printing class, but this
    // program seems too simple at the moment to warrant it.
    std::cout << R"({)" << '\n'
              << R"(    "fen": )"   << '"' << fen << '"' << ",\n"
              << R"(    "config": )"; config::print_json(std::cout); std::cout << ",\n"
              << R"(    "depth": )" << stats.depth << ",\n"
              << R"(    "hash-table MB": )" << '"' << gs.tt.get_table_bytes()/1000000 << '"' << ",\n"
              << R"(    "time-ms": )" << std::chrono::duration_cast<std::chrono::milliseconds>(stats.time).count() << ",\n"
              << R"(    "pv": )" << '"' << move::to_algebraic_long(stats.pv) << '"' << ",\n"
              << R"(    "evaluation-cp": )" << rec.eval << ",\n"
              << R"(    "nodes": )" << stats.get_nodes() << ",\n"
              << R"(    "nps": )" << stats.get_nps() << ",\n"
              << R"(    "pvnodes": )" << stats.pvnodes << ",\n"
              << R"(    "qnodes": )" << stats.qnodes << ",\n"
              << R"(    "qdepth": )" << stats.qdepth << ",\n"
              << R"(    "tt-probes": )" << stats.tt_probes << ",\n"
              << R"(    "tt-hits": )" << stats.tt_hits << ",\n"
              << R"(    "tt-hit-rate": )" << std::setprecision(2) << stats.get_tt_hit_rate() << '\n'
              << R"(})" << '\n';

    return EXIT_SUCCESS;
}