#include "uci.hpp"

#include "position/move.hpp"

#include <sstream>
#include <algorithm>

namespace uci
{

namespace
{

std::string str_tolower(std::string_view s)
{
    std::string ret(s.size(), ' ');

    std::transform(s.begin(), s.end(), ret.begin(), [](unsigned char c){ return std::tolower(c); });
    return ret;
}

}

void command::print(std::ostream& os) const
{
    os << get_id() << ' ';
    write(os);
    os << '\n' << std::flush;
}

void command_position::read(std::istream& is)
{
    std::string token;

    is >> token;
    if (token == "startpos")
    {
        bb = bitboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    else if (token == "fen")
    {
        // FEN has exactly 6 space-separated fields.
        std::ostringstream fen_stream;
        for (std::size_t i = 0; i < 6 && is >> token; i++)
            fen_stream << token << " ";

        bb = bitboard(fen_stream.str());
    }
    else
    {
        throw std::runtime_error("Invalid position format");
    }

    // Parse any moves that have taken place after the starting position.
    if (!(is >> token))
        return;
    if (token == "moves")
    {
        while (is >> token)
            moves.push_back(token);
    }
    else
    {
        throw std::runtime_error("Invalid token following position");
    }
}

void command_debug::read(std::istream& is)
{
    std::string token;

    is >> token;

    if (str_tolower(token) == "on")
        debug = true;
    else if (str_tolower(token) == "off")
    {
        debug = false;
    }
    else
    {
        throw std::runtime_error("Invalid debug state");
    }
}

void command_setoption::read(std::istream& is)
{
    std::string token;

    is >> token;
    if (token != "name")
        throw std::runtime_error("Setoption option must first be specified by a name tag");

    // Parse the option name.
    while (true)
    {
        is >> token;

        if (str_tolower(token) == "value")
            break;

        if (!name.empty())
            name.push_back(' ');
        name.append(token);

        if (!is)
            return;
    }

    // Create the value entry. Note that this has the side-effect of having a non-null but empty value
    // if we see "value" as the last word in the setoption command string.
    value.emplace();

    // Parse the option value if necessary.
    while (is)
    {
        is >> token;

        if (!value->empty())
            value->push_back(' ');
        value->append(token);
    }
}

void command_go::read(std::istream& is)
{
    std::string token;

    bool parse_searchmoves { false };

    while (true)
    {
        if (!(is >> token))
            return;
        if (token == "searchmoves")
        {
            parse_searchmoves = true;
            continue;
        }
        else if (token == "ponder")
        {
            ponder = true;
        }
        else if (token == "wtime")
        {
            is >> token;
            wtime = std::stoull(token);
        }
        else if (token == "btime")
        {
            is >> token;
            btime = std::stoull(token);
        }
        else if (token == "winc")
        {
            is >> token;
            winc = std::stoull(token);
        }
        else if (token == "binc")
        {
            is >> token;
            binc = std::stoull(token);
        }
        else if (token == "movestogo")
        {
            is >> token;
            movestogo = std::stoull(token);
        }
        else if (token == "movetime")
        {
            is >> token;
            movetime = std::stoull(token);
        }
        else if (token == "infinite")
        {
            infinite = true;
        }
        else if (token == "depth")
        {
            is >> token;
            depth = std::stoull(token);
        }
        else if (token == "nodes")
        {
            is >> token;
            nodes = std::stoull(token);
        }
        else if (token == "mate")
        {
            is >> token;
            mate = std::stoull(token);
        }
        else if (parse_searchmoves)
        {
            // If we're in searchmoves-parsing mode, just append the string to the moves list.
            searchmoves.push_back(token);
            continue;
        }
        else
        {
            throw std::runtime_error("Invalid go command");
        }

        // Reset the parse flags.
        parse_searchmoves = false;
    }
}

void command_id::write(std::ostream& os) const
{
    os << id;
}

void command_bestmove::write(std::ostream& os) const
{
    os << move_best;
    if (move_ponder.has_value())
        os << " ponder " << *move_ponder;
}

void command_option::write(std::ostream& os) const
{
    os << option;
}

}

std::istream& operator>>(std::istream& is, uci::command& v) { v.read(is); return is; }
std::ostream& operator<<(std::ostream& os, const uci::command& v) { v.write(os); return os; }