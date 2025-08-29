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

// Our global game state.
game_state gs;

// Default argument values.
constexpr std::size_t TRANSPOSITION_TABLE_MB_DEFAULT { 16 };

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

    // Print the options we support.
    {
        uci::command_option resp;
        resp.option = "name Hash type spin default " + std::to_string(TRANSPOSITION_TABLE_MB_DEFAULT) + " min 1 max 2048";
        resp.print(std::cout);
    }

    // Says we are ready to start.
    uci::command_uciok{}.print(std::cout);
}

void handle(const uci::command_isready& /*req*/)
{
    // If we haven't already initialised our transposition table, we do it here with the default 128 MB.
    if (search::transposition_table.get_table_bytes() == 0)
        search::transposition_table.set_table_bytes(1000000ULL*TRANSPOSITION_TABLE_MB_DEFAULT);

    // Say we are ready.
    uci::command_readyok{}.print(std::cout);
}

void handle(const uci::command_ucinewgame& /*req*/)
{
    // Clear the game state.
    gs = game_state{};
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
    // We use copy-assignment of the position only so we don't clear othe meta-data that affects search (e.g. hash-age).
    gs = req.bb;

    // Apply the subsequent moves and increment the root ply if necessary.
    for (const auto& move_str : req.moves)
    {
        std::uint32_t move { move::from_algebraic_long(move_str, gs.bb) };
        make_move({ .check_legality = false }, gs, move);
        gs.root_ply++;
    }
}

void handle(const uci::command_go& /*req*/)
{
    // Just calculate to depth 6 no matter what (WayChess 1.0.0 behaviour).
    const std::uint32_t recommended_move = search::recommend_move(gs, &search::search_negamax, 6, evaluation::raw_material).move;

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
