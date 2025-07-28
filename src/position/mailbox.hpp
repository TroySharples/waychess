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

    // Castling nibble - same as in the bitboard representation.
    std::uint8_t castling;

    // The number of ply since the game started. An even ply implies white is to move. For initial
    // positions where black starts, this should be initialised to 1.
    std::uint16_t ply_counter;

    constexpr bool is_black_to_play() const noexcept { return ply_counter & 1; }
    constexpr bool is_white_to_play() const noexcept { return !is_black_to_play(); }

    // The number of ply since the last pawn-push or capture (used for 50 move drawing rule).
    std::uint8_t ply_50m;

    // Print the corresponding fen string.
    std::string get_fen_string() const noexcept;

    // Print the corresponding unicode board.
    std::ostream& display_unicode_board(std::ostream& os, bool flipped = false) const noexcept;
};
