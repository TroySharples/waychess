#include "search/algorithms.hpp"
#include "utility/uci.hpp"
#include "utility/logging.hpp"
#include "version.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

namespace
{

// Our global bitboard - we'll think about wrapping this in a nice class at some point.
bitboard bb;

void handle(const uci::command_uci& /*req*/)
{
    // Print the WhyChess ID.
    {
        uci::command_id resp;
        resp.id = "name WhyChess " + std::string(VERSION);
        resp.print(std::cout);
    }
    {
        uci::command_id resp;
        resp.id = "author " + std::string(AUTHOR);
        resp.print(std::cout);
    }

    // We don't support any options.

    // Says we are ready to start.
    uci::command_uciok{}.print(std::cout);
}

void handle(const uci::command_isready& /*req*/)
{
    uci::command_readyok{}.print(std::cout);
}

void handle(const uci::command_ucinewgame& /*req*/)
{
    // Clear the position.
    bb = bitboard{};
}

void handle(const uci::command_setoption& /*req*/)
{
    // We currently don't support any options - everything just gets ignored.
}

void handle(const uci::command_debug& req)
{
    // Non-fatal error if we are requesting debug info - we'll add this into the program at some later time.
    if (req.debug)
        std::cerr << "We do not currently support the debug command" << std::endl;
}

void handle(const uci::command_position& req)
{
    bb = req.bb;

    // Apply the subsequent moves if necessary.
    for (const auto& move_str : req.moves)
    {
        std::uint32_t move { move::from_algebraic_long(move_str, bb) };
        make_move({ .check_legality = false }, bb, move);
    }
}

void handle(const uci::command_go& req)
{
    const auto time_start = std::chrono::steady_clock::now();

    // Just calculate to depth 6 no matter what (WayChess 1.0.0 behaviour).
    const std::uint32_t recommended_move = search::recommend_move(bb, search::negamax_prune, 6, evaluation::raw_material).move;

    if (bb.is_black_to_play() ? req.btime.has_value() : req.wtime.has_value())
    {
        // If we've been given our time parameters, we pretend to spend 1/20 of our remaining time and half our increment thinking.
        const std::size_t remaining_ms { bb.is_black_to_play() ? *req.btime : *req.wtime };
        const std::size_t increment_ms { bb.is_black_to_play() ? (req.binc.has_value() ? *req.binc : 0)  : (req.winc.has_value() ? *req.winc : 0) };

        const std::chrono::milliseconds time((remaining_ms/20) + (increment_ms/2));
        std::this_thread::sleep_until(time_start + time);
    }

    // Print our recommendation.
    {
        uci::command_bestmove resp;
        resp.move_best = move::to_algebraic_long(recommended_move);
        resp.print(std::cout);
    }
}

void handle(std::string_view command)
{
    // Non-fatal warning that we do not support this command.
    std::cerr << "Unknown command - " << command << std::endl;
}

}

int main()
{
    // Setup the logger.
    set_log_method(log_method::uci);

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci")
        {
            uci::command_uci req;
            req.read(iss);
            handle(req);
        }
        else if (command == "isready")
        {
            uci::command_isready req;
            req.read(iss);
            handle(req);
        }
        else if (command == "ucinewgame")
        {
            uci::command_ucinewgame req;
            req.read(iss);
            handle(req);
        }
        else if (command == "setoption")
        {
            uci::command_setoption req;
            req.read(iss);
            handle(req);
        }
        else if (command == "debug")
        {
            uci::command_debug req;
            req.read(iss);
            handle(req);
        }
        else if (command == "position")
        {
            uci::command_position req;
            req.read(iss);
            handle(req);
        }
        else if (command == "go")
        {
            uci::command_go req;
            req.read(iss);
            handle(req);
        }
        else if (command == "quit")
        {
            return EXIT_SUCCESS;
        }
        else
        {
            handle(command);
        }
    }

    return EXIT_SUCCESS;
}
