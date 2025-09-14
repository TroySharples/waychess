#pragma once

#include "position/bitboard.hpp"

#include <optional>

namespace uci
{

// Virtual command base-class.
struct command
{
    virtual ~command() = default;

    virtual constexpr const char* get_id() const noexcept = 0;

    virtual void read(std::istream& is) = 0;
    virtual void write(std::ostream& os) const = 0;

    void print(std::ostream& os) const;

    friend std::istream& operator>>(std::istream& is, command& v);
    friend std::ostream& operator<<(std::ostream& os, const command& v);
};

/***********************************************
* GUI TO ENGINE
*
* We only implement the read methods.
***********************************************/

// Just tells the engine to use UCI (all we support anyway) - contains no body.
struct command_uci : command
{
    static constexpr const char* ID { "uci" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Tells the engine that we are starting a new game - contains no body.
struct command_ucinewgame : command
{
    static constexpr const char* ID { "ucinewgame" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Ping from the GUI - contains no body.
struct command_isready : command
{
    static constexpr const char* ID { "isready" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Sets the position the engine should consider, optionally along with a sequence of moves
// that are played before the engine takes over.
struct command_position : command
{
    static constexpr const char* ID { "position" };
    constexpr const char* get_id() const noexcept override { return ID; }

    bitboard bb;
    std::vector<std::string> moves;

    void read(std::istream& is) override;
    void write(std::ostream& /*os*/) const override { };
};

// Sent from the GUI to the engine to configure debug mode.
struct command_debug : command
{
    static constexpr const char* ID { "debug" };
    constexpr const char* get_id() const noexcept override { return ID; }

    bool debug;

    void read(std::istream& is) override;
    void write(std::ostream& /*os*/) const override { };
};

// Sent from the GUI to the engine to set a configurable option.
struct command_setoption : command
{
    static constexpr const char* ID { "setoption" };
    constexpr const char* get_id() const noexcept override { return ID; }

    std::string name;
    std::optional<std::string> value;

    void read(std::istream& is) override;
    void write(std::ostream& /*os*/) const override { };
};

// Sent from the GUI to tell the engine to start calculating the current position.
struct command_go : command
{
    static constexpr const char* ID { "go" };
    constexpr const char* get_id() const noexcept override { return ID; }

    std::vector<std::string> searchmoves;

    bool ponder;

    std::optional<std::size_t> wtime, btime, winc, binc, movestogo, movetime;
    bool infinite;

    // Returns 0 if the increment isn't set.
    std::size_t get_increment_ms(bool is_black) const noexcept;

    std::optional<std::size_t> depth, nodes, mate;

    void read(std::istream& is) override;
    void write(std::ostream& /*os*/) const override { };
};

// Sent from the GUI to the engine to stop current search - contains no body.
struct command_stop : command
{
    static constexpr const char* ID { "stop" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Sent from the GUI to tell the engine that the opponent has played the expected move - contains no body.
struct command_ponderhit : command
{
    static constexpr const char* ID { "ponderhit" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Sent from the GUI to the engine to quit the program - contains no body.
struct command_quit : command
{
    static constexpr const char* ID { "quit" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

/***********************************************
* ENGINE TO GUI
*
* We only implement the write methods.
***********************************************/

// Initialisation confirmation from the engine - contains no body.
struct command_uciok : command
{
    static constexpr const char* ID { "uciok" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Pong from the engine send in response to a isready - contains no body.
struct command_readyok : command
{
    static constexpr const char* ID { "readyok" };
    constexpr const char* get_id() const noexcept override { return ID; }

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& /*os*/) const override { };
};

// Contains either the name or the author - multiple ID messages can be sent.
struct command_id : command
{
    static constexpr const char* ID { "id" };
    constexpr const char* get_id() const noexcept override { return ID; }

    std::string id;

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& os) const override;
};

// Recommend move in position, and optionally also a move to ponder on.
struct command_bestmove : command
{
    static constexpr const char* ID { "bestmove" };
    constexpr const char* get_id() const noexcept override { return ID; }

    std::string move_best;
    std::optional<std::string> move_ponder;

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& os) const override;
};

// Report a configurable parameter to the GUI. We could really overcomplicate things here, but as for our use-case
// these are all just going to be static strings this is probably okay.
struct command_option : command
{
    static constexpr const char* ID { "option" };
    constexpr const char* get_id() const noexcept override { return ID; }

    std::string option;

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& os) const override;
};

// Logs the engine can send to the GUI. There doesn't seem to be a strict format to these logs, so we'll just
// try our best when reporting the logs.
struct command_info : command
{
    static constexpr const char* ID { "info" };
    constexpr const char* get_id() const noexcept override { return ID; }

    std::string info;

    void read(std::istream& /*is*/) override { };
    void write(std::ostream& os) const override;
};

}