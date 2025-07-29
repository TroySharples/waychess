#include "position/bitboard.hpp"

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

void handle_debug(std::istringstream& /*iss*/)
{
    std::cerr << "We do not support the debug command" << std::endl;
}

void handle_position(std::istringstream& iss)
{
    std::string fen;
    std::getline(iss >> std::ws, fen);

    bitboard bb(fen);
    bb.display_unicode_board(std::cout) << std::endl;
}

void handle_go(std::istringstream& iss)
{
    
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
