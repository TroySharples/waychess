#pragma once

#include "pieces/pieces.hpp"

#include <array>
#include <span>

struct mailbox;

// This is the main structure that is used for storing the position when generating and
// evaluating moves. It is based around an array of 14 bitboards, although we might experiment
// with other layouts in the future.
struct bitboard
{
    bitboard() = default;

    // Construct from mailbox representation.
    bitboard(const mailbox& mb);

    // Construct from fen-string.
    bitboard(std::string_view fen);

    // An array of bitboards (indexed by the piece-idx enum). This includes bitboards for
    // all white and all black pieces.
    std::array<std::uint64_t, 14> boards;

    constexpr std::span<std::uint64_t> get_white_boards() noexcept { return { &boards[piece_idx::w_pawn], 6 }; }
    constexpr std::span<std::uint64_t> get_black_boards() noexcept { return { &boards[piece_idx::b_pawn], 6 }; }

    constexpr std::span<const std::uint64_t> get_white_boards() const noexcept { return { &boards[piece_idx::w_pawn], 6 }; }
    constexpr std::span<const std::uint64_t> get_black_boards() const noexcept { return { &boards[piece_idx::b_pawn], 6 }; }
    
    // Contains at most a single one-bit, which is the en-passent target square created
    // by a possible double-pawn push on the last ply.
    std::uint64_t en_passent_bb;

    // Castling nibble.
    std::uint8_t castling;
    static constexpr std::uint8_t CASTLING_W_KS { 0b0001 };
    static constexpr std::uint8_t CASTLING_W_QS { 0b0010 };
    static constexpr std::uint8_t CASTLING_B_KS { 0b0100 };
    static constexpr std::uint8_t CASTLING_B_QS { 0b1000 };

    // The number of ply since the game started. An even ply implies white is to move. For initial
    // positions where black starts, this should be initialised to 1.
    std::uint16_t ply_counter;

    // The number of ply since the last pawn-push or capture (used for 50 move drawing rule).
    std::uint8_t ply_50m;

    bool is_black_to_play() const noexcept { return ply_counter & 1; }
    bool is_white_to_play() const noexcept { return !is_black_to_play(); }

    // Print the corresponding fen string.
    std::string get_fen_string() const noexcept;

    // Prints board using unicode chess piece symbols.
    friend std::ostream& operator<<(std::ostream& os, const bitboard& v);
};
