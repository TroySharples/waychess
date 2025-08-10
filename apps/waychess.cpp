#include "search/algorithms.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

namespace
{

// Our global bitboard - we'll think about wrapping this in a nice class at some point.
bitboard bb;

void handle_uci()
{
    std::cout << "id name Waychess\n"
              << "id author Troy Sharples\n"
              << "uciok" << std::endl;
}

void handle_isready()
{
    std::cout << "readyok" << std::endl;
}

void handle_ucinewgame()
{

}

void handle_setoption(std::istringstream& iss)
{
    std::string option;
    iss >> option;

    if (option == "Hash")
    {
        // We will need to implement this one day.
    }
    else
    {
        // Ignore unhandled options.
    }

}

void handle_debug(std::istringstream& /*iss*/)
{
    std::cerr << "We do not support the debug command" << std::endl;
}

void handle_position(std::istringstream& iss)
{
    std::string token;

    // Parse starting postion.
    iss >> token;
    if (token == "startpos")
    {
        bb = bitboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    else if (token == "fen")
    {
        // FEN has exactly 6 space-separated fields.
        std::ostringstream fen_stream;
        for (std::size_t i = 0; i < 6 && iss >> token; i++)
            fen_stream << token << " ";

        bb = bitboard(fen_stream.str());
    }
    else
    {
        std::cerr << "Invalid position format." << std::endl;
    }

    if (!iss)
        return;

    // Parse any moves that have taken place after the starting position.
    iss >> token;
    if (token == "moves")
    {
        while (iss >> token)
        {
            std::uint32_t move { move::from_algebraic_long(token, bb) };
            make_move({ .check_legality = false }, bb, move);
        }
    }
    else
    {
        std::cerr << "Invalid token following position." << std::endl;
    }
}

struct go_parameters
{
    std::size_t wtime {}, btime {};

    friend std::istream& operator>>(std::istream& is, go_parameters& v);
};

std::istream& operator>>(std::istream& is, go_parameters& v)
{
    while (is)
    {
        std::string token;
        is >> token;
        if (token == "wtime")
        {
            is >> token;
            v.wtime = std::stoull(token);
        }
        else if (token == "btime")
        {
            is >> token;
            v.btime = std::stoull(token);
        }
        else
        {
            std::cerr << "Unknown go-parameter " << token << " - ";
            is >> token;
            std::cerr << token << std::endl;
        }
    }

    return is;
}

void handle_go(std::istringstream& iss)
{
    std::uint32_t best_move;

    go_parameters param;
    iss >> param;

    if (const std::size_t remaining_ms = (bb.is_black_to_play() ? param.btime : param.wtime); remaining_ms)
    {
        // If we know the time limit, just spend 1/15 the time playing the actual move.
        const std::chrono::milliseconds time(remaining_ms / 15);
        best_move = search::recommend_move_id(bb, search::negamax, time, evaluation::raw_material).move;
    }
    else
    {
        // Otherwise just calculate to depth 6.
        best_move = search::recommend_move(bb, search::negamax, 6, evaluation::raw_material).move;
    }

    std::cout << "bestmove " << move::to_algebraic_long(best_move) << std::endl;
}

void handle_unknown(std::string_view command)
{
    std::cerr << "Unknown command - " << command << std::endl;
}

}

int main()
{
    std::string line;

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci")
            handle_uci();
        else if (command == "isready")
            handle_isready();
        else if (command == "ucinewgame")
            handle_ucinewgame();
        else if (command == "setoption")
            handle_setoption(iss);
        else if (command == "debug")
            handle_debug(iss);
        else if (command == "position")
            handle_position(iss);
        else if (command == "go")
            handle_go(iss);
        else if (command == "quit")
            return EXIT_SUCCESS;
        else
            handle_unknown(command);
    }

    return EXIT_SUCCESS;
}
