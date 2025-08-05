#include "position/generate_moves.hpp"
#include "position/make_move.hpp"
#include "position/move.hpp"
#include "utility/perft.hpp"

#include <bits/chrono.h>
#include <cstdlib>
#include <string>
#include <chrono>
#include <cstring>
#include <unistd.h>

static std::ostream& print_usage(const char* argv0, std::ostream& os)
{
    return os << "Usage: " << argv0 << " <options>\n"
              << "    Options:\n"
              << "         -h                   -> Print this help menu.\n"
              << "         -t                   -> Also print the first level tree of possible moves.\n"
              << "         -y                   -> Also print the final time taken for the perft test in ms.\n"
              << "         -f [fen]             -> The FEN string for the starting position. Optional, defaults to starting position.\n"
              << "         -d [depth]           -> The perft depth. Optional, default 1.\n"
              << "         -s [stratagy]        -> The perft search strategy. Can be either copy-no-hash, unmake-no-hash, copy-hash, or unmake-hash - default unmake-hash.\n"
              << "         -k [hash-table size] -> The size of the hash-table (in MiB) if used. Optional, default 1000.\n";
}

int main(int argc, char** argv)
{
    // Default arguments.
    bool help                         { false };
    bool tree                         { false };
    bool time                         { false };
    std::string fen                   { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
    perft_args args                   { .depth=1, .strategy=perft_args::unmake_hash };
    std::size_t hash_table_size_bytes { 1000000000ULL };

    // Parse options.
    for (int c; (c = getopt(argc, argv, "htyf:d:s:k:")) != -1; )
    {
        switch (c)
        {
            // Help.
            case 'h':
            {
                help = true;
                break;
            }
            // Tree.
            case 't':
            {
                tree = true;
                break;
            }
            // Show time.
            case 'y':
            {
                time = true;
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
                args.depth = std::stoull(optarg);
                break;
            }
            // Strategy.
            case 's':
            {
                args.strategy = perft_args::strategy_type_from_string(optarg);
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
                if (optopt == 'f' || optopt == 'd' || optopt == 's' || optopt == 'k')
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

    // Check option consistency.
    if (args.depth == 0 && tree)
    {
        std::cerr << "Depth must cannot be 0 when tree-printing is enabled.\n";
        print_usage(argv[0], std::cerr);
        return EXIT_FAILURE;
    }

    // Start printing the JSON. We might clean this up later by having a proper JSON printing class, but this
    // program seems too simple at the moment to warrent it.
    std::cout << R"({)" << '\n'
              << R"(    "fen": )"   << '"' << fen << '"' << ",\n"
              << R"(    "depth": )" << args.depth << ",\n"
              << R"(    "strategy": )" << '"' << perft_args::strategy_type_to_string(args.strategy) << '"' << ",\n";

    const bitboard position_start(fen);
    std::size_t total_nodes {};

    if (args.strategy == perft_args::copy_hash || args.strategy == perft_args::unmake_hash)
    {
        set_perft_hash_table_bytes(hash_table_size_bytes);
        std::cout << R"(    "hash-table MB": )"   << '"' << get_perft_hash_table_bytes()/1000000 << '"' << ",\n";
    }

    const auto time_start = std::chrono::steady_clock::now();
    if (tree)
    {
        std::cout << R"(    "tree": [)" << '\n';

        bool array_first { true };
        std::array<std::uint32_t, 256> move_buf;
        const std::size_t moves { generate_pseudo_legal_moves(position_start, move_buf) };
        for (std::size_t i = 0; i < moves; i++)
        {
            bitboard next_position { position_start };

            if (!make_move({ .check_legality = true }, next_position, move_buf[i]))
                continue;

            const std::size_t nodes { perft({ .depth=args.depth-1, .strategy=args.strategy }, next_position) };
            total_nodes += nodes;

            // Handle trailing-comma printing.
            if (array_first)
                array_first = false;
            else
                std::cout << ",\n";

            std::cout << R"(        {)" << '\n';
            std::cout << R"(            "move": ")" << move::to_algebraic_long(move_buf[i]) << R"(",)" << '\n';
            std::cout << R"(            "nodes": )" << nodes << '\n';
            std::cout << R"(        })";
        }
        std::cout << '\n' << R"(    ],)" << '\n';
    }
    else
    {
        total_nodes = perft(args, position_start);
    }
    const auto time_end = std::chrono::steady_clock::now();

    // Print the time and nps.
    if (time)
    {
        const std::uint64_t duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
        std::cout << R"(    "time-ms": )" << duration_ms << ",\n"
                  << R"(    "nps": )"     << 1000ULL * total_nodes / duration_ms << ",\n";
    }

    // Finally print the total number of nodes.
    std::cout << R"(    "total-nodes": )" << total_nodes << '\n'
              << R"(})" << '\n';

    return EXIT_SUCCESS;
}