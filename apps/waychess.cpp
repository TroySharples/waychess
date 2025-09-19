#include "config.hpp"
#include "utility/game.hpp"
#include "utility/uci.hpp"
#include "utility/logging.hpp"
#include "position/make_move.hpp"
#include "version.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

namespace
{

// Default argument values.
constexpr std::size_t TRANSPOSITION_TABLE_MB_DEFAULT { 128 };

void handle(game& /*g*/, const uci::command_uci& /*req*/)
{
    // Print the WayChess ID.
    {
        uci::command_id resp;
        resp.id = "name " + std::string(NAME) + ' ' + std::string(VERSION);
        resp.print(std::cout);
    }
    {
        uci::command_id resp;
        resp.id = "author " + std::string(AUTHOR);
        resp.print(std::cout);
    }

    // Print our config information.
    {
        uci::command_info resp;
        {
            std::ostringstream ss;
            ss << "config ";
            config::print_tv(ss);
            resp.info = ss.str();
        }
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

void handle(game& g, const uci::command_isready& /*req*/)
{
    // If we haven't already initialised our transposition table, we do it here with the default 128 MB.
    if (g.gs.tt.get_table_bytes() == 0)
        g.gs.tt.set_table_bytes(1000000ULL*TRANSPOSITION_TABLE_MB_DEFAULT);

    // Say we are ready.
    uci::command_readyok{}.print(std::cout);
}

void handle(game& g, const uci::command_ucinewgame& /*req*/)
{
    // Reset the game state.
    g.gs.reset();
}

void handle(game& g, const uci::command_setoption& req)
{
    if (req.name == "Hash")
    {
        if (!req.value.has_value())
            throw std::runtime_error("Set hash option must contain a value");

        const std::size_t hash_bytes { 1000000ULL * std::stoull(*req.value) };

        // This should only affect the search hash-table.
        g.gs.tt.set_table_bytes(hash_bytes);
    }
    else
    {
        // Ignore unhandled options.
    }
}

void handle(game& /*g*/, const uci::command_debug& req)
{
    // Non-fatal error if we are requesting debug info - we'll add this into the program at some later time.
    if (req.debug)
        std::cerr << "We do not currently support the debug command" << std::endl;
}

void handle(game& g, const uci::command_position& req)
{
    // We use copy-assignment of the position only so we don't clear othe meta-data that affects search (e.g. hash-age).
    g.gs.load(req.bb);

    // Apply the subsequent moves and increment the root ply if necessary.
    for (const auto& move_str : req.moves)
    {
        std::uint64_t move { move::from_algebraic_long(move_str, g.gs.bb) };
        make_move({ .check_legality = false }, g.gs, move);
    }
}

void handle(game& g, const uci::command_go& req)
{
    const bool is_black_to_play { g.gs.bb.is_black_to_play() };
    if (is_black_to_play ? !req.btime.has_value() : !req.wtime.has_value())
    {
        // Calculate infinitely if we haven't been told our time parameters.
        g.search(game::search_go, 64);
    }
    else
    {
        // Otherwise, we just spend 1/20 of our remaining time and half our increment calculating this.
        const std::size_t remaining_ms { is_black_to_play ? *req.btime : *req.wtime };
        const std::size_t increment_ms { req.get_increment_ms(is_black_to_play) };

        const std::chrono::milliseconds time((remaining_ms/20) + (increment_ms/2));
        g.search(game::search_go, 64, time);
    }
}

void handle(game& g, const uci::command_evaluate& req)
{
    g.search(game::search_evaluate, req.depth);
}

void handle(game& g, const uci::command_stop& /*req*/)
{
    g.stop();
}

void handle(game& g, const uci::command_quit& /*req*/)
{
    handle(g, uci::command_stop{});
}

void handle(game& /*g*/, std::string_view command)
{
    // Non-fatal warning that we do not support this command.
    std::cerr << "Unknown command - " << command << std::endl;
}

}

int main() try
{
    // Our global game.
    game g;

    // Setup the logger.
    set_log_method(log_method::uci);

    // Initialise our best-move callback.
    g.callback_best_move = [] (std::uint64_t move)
    {
        // Make sure that our best move is non-null (this would happen if the
        // starting position is in checkmate for example).
        if (move == move::NULL_MOVE)
            throw std::runtime_error("Recommended move is null");

        // Actually print the best move.
        uci::command_bestmove resp;
        resp.move_best = move::to_algebraic_long(move);
        resp.print(std::cout);
    };

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == uci::command_uci::ID)
        {
            uci::command_uci req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_isready::ID)
        {
            uci::command_isready req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_ucinewgame::ID)
        {
            uci::command_ucinewgame req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_setoption::ID)
        {
            uci::command_setoption req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_debug::ID)
        {
            uci::command_debug req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_position::ID)
        {
            uci::command_position req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_go::ID)
        {
            uci::command_go req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_evaluate::ID)
        {
            uci::command_evaluate req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_stop::ID)
        {
            uci::command_stop req;
            req.read(iss);
            handle(g, req);
        }
        else if (command == uci::command_quit::ID)
        {
            uci::command_quit req;
            req.read(iss);
            handle(g, req);

            return EXIT_SUCCESS;
        }
        else
        {
            handle(g, command);
        }
    }

    return EXIT_SUCCESS;
}
catch(const std::runtime_error& e)
{
    std::cerr << "Encountered fatal error - " << e.what() << std::endl;
    return EXIT_FAILURE;
}
