#include "position/search.hpp"

#include <chrono>
#include <cstring>
#include <unistd.h>

static std::ostream& print_usage(const char* argv0, std::ostream& os)
{
    return os << "Usage: " << argv0 << " <options>\n"
              << "    Options:\n"
              << "         -h         -> Print this help menu.\n"
              << "         -b         -> Also print the recommended move.\n"
              << "         -f [fen]   -> The FEN string for the starting position. Optional, defaults to starting position.\n"
              << "         -d [depth] -> The evaluation depth. Optional, default 1.\n";
}

int main(int argc, char** argv)
{
    // Default arguments.
    bool help         { false };
    bool recommened   { false };
    std::string fen   { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
    std::size_t depth { 1 };

    // Parse options.
    for (int c; (c = getopt(argc, argv, "hbf:d:")) != -1; )
    {
        switch (c)
        {
            // Help.
            case 'h':
            {
                help = true;
                break;
            }
            // Recommended move.
            case 'b':
            {
                recommened = true;
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

    const bitboard position_start(fen);
    std::int16_t evaluation_cp {};

    const auto time_start = std::chrono::steady_clock::now();
    if (recommened)
    {
        const auto best_move = best_move_minimax(position_start, depth, &evaluate_terminal);
        evaluation_cp = best_move.second;

        std::cout << R"(    "recommendation": )" << '"' << move::to_algebraic_long(best_move.first) << '"' << ",\n";
    }
    else
    {
        evaluation_cp = evaluate_minimax(position_start, depth, &evaluate_terminal);
    }
    const auto time_end = std::chrono::steady_clock::now();

    // Start printing the JSON file in one go. We might clean this up later by having a proper JSON printing class, but this
    // program seems too simple at the moment to warrent it.
    std::cout << R"({)" << '\n'
              << R"(    "fen": )"   << '"' << fen << '"' << ",\n"
              << R"(    "depth": )" << depth << ",\n"
              << R"(    "search": )" << '"' << "minimax" << '"' << ",\n"
              << R"(    "terminal-evaluation": )" << '"' << "raw-material" << '"' << ",\n"
              << R"(    "time-ms": )" << std::chrono::duration_cast<std::chrono::milliseconds>(time_end-time_start).count() << ",\n"
              << R"(    "evaluation-cp": )" << evaluation_cp << '\n'
              << R"(})" << '\n';

    return EXIT_SUCCESS;
}