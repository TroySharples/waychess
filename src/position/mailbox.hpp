#pragma once

#include "pieces/pieces.hpp"

#include <array>
#include <optional>

struct bitboard;

// This is a simpler and slower board representation than the bitboard type. This is
// primarily used for board serialisation and deserialisation.
struct mailbox
{
    mailbox() = default;

    // Construct from bitboard representation.
    mailbox(const bitboard& bb);

    // Construct from fen-string.
    mailbox(std::string_view fen);

    // Each entry in the array represents what is on each square. Index 0 represents
    // a1 and index 63 represents h8.
    std::array<piece_idx, 64> squares;

    // The location (if any) of the en-passent target square.
    std::optional<std::uint8_t> en_passent_square;

    // Castling options.
    bool castling_w_ks;
    bool castling_w_qs;
    bool castling_b_ks;
    bool castling_b_qs;

    // The number of ply since the game started. An even ply implies white is to move. For initial
    // positions where black starts, this should be initialised to 1.
    std::uint16_t ply_counter;

    bool is_black_to_play() const noexcept { return ply_counter & 1; }
    bool is_white_to_play() const noexcept { return !is_black_to_play(); }

    // The number of ply since the last pawn-push or capture (used for 50 move drawing rule).
    std::uint8_t ply_50m;

    // Print the corresponding fen string.
    std::string get_fen_string() const noexcept;

    // Prints board using unicode chess piece symbols.
    friend std::ostream& operator<<(std::ostream& os, const mailbox& v);
};
