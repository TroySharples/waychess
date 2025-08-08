#include "utility/puzzle.hpp"
#include "position/search.hpp"

#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <unistd.h>

static std::ostream& print_usage(const char* argv0, std::ostream& os)
{
    return os << "Usage: " << argv0 << " <options>\n"
              << "    Options:\n"
              << "         -h         -> Print this help menu.\n"
              << "         -f [file]  -> The path to the Lichess CSV containing the puzzles.\n"
              << "         -d [depth] -> The perft depth. Optional, default 4.\n";
}

int main(int argc, char** argv)
{
    // Default arguments.
    bool help { false };
    std::filesystem::path csv_path;
    std::uint8_t depth { 4 };

    // Parse options.
    for (int c; (c = getopt(argc, argv, "hyf:d:")) != -1; )
    {
        switch (c)
        {
            // Help.
            case 'h':
            {
                help = true;
                break;
            }
            // CSV path.
            case 'f':
            {
                csv_path = optarg;
                break;
            }
            // Depth.
            case 'd':
            {
                depth = std::stoul(optarg);
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

    std::ifstream is(csv_path);
    if (!is)
    {
        std::cerr << "Could not open specified CSV file.\n";
        return EXIT_FAILURE;
    }

    // Skip the CSV column names.
    std::string line;
    if (!std::getline(is, line))
    {
        std::cerr << "Error reading CSV file.\n";
        return EXIT_FAILURE;
    }

    recommendation rec = [] (const bitboard& bb, const void* args) {
        return best_move_minimax(bb, *static_cast<const std::uint8_t*>(args), &evaluate_terminal).first;
    };

    std::size_t puzzles_total {};
    std::size_t puzzles_solved {};

    const auto time_start = std::chrono::steady_clock::now();
    while (std::getline(is, line))
    {
        std::stringstream ss(line);

        puzzle p;
        ss >> p;

        puzzles_total++;
        if (p.solve(rec, &depth)) puzzles_solved++;
    }
    const auto time_end = std::chrono::steady_clock::now();

    const double puzzles_accuracy = static_cast<double>(puzzles_solved) / static_cast<double>(puzzles_total);

    // Start printing the JSON. We only do this at the end in case we have trouble parsing the CSV file when running the test.
    std::cout << R"({)" << '\n'
              << R"(    "file": )"   << csv_path.filename() << ",\n"
              << R"(    "depth": )" << static_cast<int>(depth) << ",\n"
              << R"(    "search": )" << '"' << "minimax" << '"' << ",\n"
              << R"(    "terminal-evaluation": )" << '"' << "raw-material" << '"' << ",\n"
              << R"(    "time-ms": )" << std::chrono::duration_cast<std::chrono::milliseconds>(time_end-time_start).count() << ",\n"
              << R"(    "puzzles-total": )" << puzzles_total << ",\n"
              << R"(    "puzzles-solved": )" << puzzles_solved << ",\n"
              << R"(    "puzzles-accuracy": )" << std::setprecision(3) << puzzles_accuracy << '\n'
              << R"(})" << '\n';

    return EXIT_SUCCESS;
}