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

    // Gets the piece type on a particular square. The colour variant assumes the colour of the
    // piece is known beforehand.
    constexpr piece_idx get_piece_type(std::uint8_t bb) const noexcept;
    constexpr piece_idx get_piece_type_colour(std::uint8_t bb, bool is_black) const noexcept;

    constexpr bool is_black_to_play() const noexcept { return ply_counter & 1; }
    constexpr bool is_white_to_play() const noexcept { return !is_black_to_play(); }

    // Print the corresponding fen string.
    std::string get_fen_string() const noexcept;

    // Print the corresponding unicode board.
    std::ostream& display_unicode_board(std::ostream& os, bool flipped = false) const noexcept;

    // Equality overloads
    bool operator==(const bitboard& other) const noexcept
    {
        return boards == other.boards
            && en_passent_bb == other.en_passent_bb
            && castling      == other.castling
            && ply_counter   == other.ply_counter
            && ply_50m       == other.ply_50m;
    }
    bool operator!=(const bitboard& other) const noexcept { return !this->operator==(other); }
};

constexpr piece_idx bitboard::get_piece_type(std::uint8_t bb) const noexcept
{
    return get_piece_type_colour(bb, boards[piece_idx::b_any] & bb);
}

constexpr piece_idx bitboard::get_piece_type_colour(std::uint8_t bb, bool is_black) const noexcept
{
    for (std::uint8_t idx = (is_black ? piece_idx::w_pawn : piece_idx::b_pawn); idx <= (is_black ? piece_idx::w_queen : piece_idx::b_queen); idx++)
        if (boards[idx] & bb)
            return static_cast<piece_idx>(idx);
    return piece_idx::empty;

}
