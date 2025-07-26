#include "position/bitboard.hpp"
#include "position/move.hpp"
#include "position/make_move.hpp"

#include <optional>
#include <cstring>
#include <unistd.h>

static std::ostream& print_usage(const char* argv0, std::ostream& os)
{
    return os << "Usage: " << argv0 << " <options>\n"
              << "    Options:\n"
              << "         -h               -> Print this help menu.\n"
              << "         -f [fen]         -> The FEN string for the starting position. Required, no default.\n"
              << "         -t [format]      -> The format of the output. Can be either fen (default), unicode, or side (just prints the side to play).\n"
              << "         -o [orientation] -> When format is unicode, choose the orientation the board should be displayed. Optional, default white. Has\n"
              << "                             no affect when printing the FEN string.\n"
              << "         -m [move]        -> Instead print the resulting position after the move (specified in algebraic long format). Optional, default\n"
              << "                             to just printing the input position. This must be specified after the FEN input.\n";
}

int main(int argc, char** argv)
{
    const std::string usage_str { std::string("    Usage - ") + argv[0] + "<options>" };

    // Default arguments.
    bool help { false };
    enum { fen, unicode, side } format { fen };
    bool flipped { fen };
    std::optional<std::string> input;
    std::optional<std::uint32_t> move;

    // Parse options.
    for (int c; (c = getopt(argc, argv, "hf:t:o:m:")) != -1; )
    {
        switch (c)
        {
            // Help.
            case 'h':
            {
                help = true;
                break;
            }
            // FEN input.
            case 'f':
            {
                input = optarg;
                break;
            }
            // Output format.
            case 't':
            {
                if (std::strcmp(optarg, "fen") == 0)
                {
                    format = fen;
                }
                else if (std::strcmp(optarg, "unicode") == 0)
                {
                    format = unicode;
                }
                else if (std::strcmp(optarg, "side") == 0)
                {
                    format = side;
                }
                else
                {
                    std::cerr << "Unvalid output format.\n";
                    return EXIT_FAILURE;
                }
                break;
            }
            // Orientation.
            case 'o':
            {
                if (std::strcmp(optarg, "white") == 0)
                {
                    flipped = false;
                }
                else if (std::strcmp(optarg, "black") == 0)
                {
                    flipped = true;
                }
                else
                {
                    std::cerr << "Unvalid board orientation.\n";
                    return EXIT_FAILURE;
                }
                break;
            }
            // Move.
            case 'm':
            {
                if (!input.has_value())
                {
                    std::cerr << "FEN input must be specified before move.\n";
                    return EXIT_FAILURE;
                }

                move = move::from_algebraic_long(optarg, bitboard{ *input });
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

    // We must have an input.
    if (!input.has_value())
    {
        std::cerr << "FEN input must be specified.\n";
        return EXIT_FAILURE;
    }

    bitboard bb { *input };

    // Applies the move if necessary.
    if (move.has_value())
    {
        if (!make_move({ .check_legality=true }, bb, *move))
        {
            std::cerr << "Requested move is illegal in this position.\n";
            return EXIT_FAILURE;
        }
    }

    // Prints the board in one of two possible ways.
    switch (format)
    {
        case fen:     std::cout << bb.get_fen_string() << '\n'; break;
        case unicode: bb.display_unicode_board(std::cout, flipped); break;
        case side:    std::cout << (bb.is_black_to_play() ? "white\n" : "black\n"); break;
    }

    return EXIT_SUCCESS;
}