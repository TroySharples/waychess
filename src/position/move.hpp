#pragma once

#include "pieces/pieces.hpp"

// Moves are serialised as 32-bit integers - 16 bits to encode the mechanics of a move, and
// an additional 16 bits to contain meta-data about the move type.
//      0 -  5 : from-square-idx
//      6 - 11 : to-square-idx
//     12 - 15 : piece-idx
//     16 - 31 : move-type

constexpr std::uint32_t serialise_move(std::uint8_t from_square, std::uint8_t to_square, piece_idx p, std::uint16_t move_type = 0)
{
    return from_square | (to_square << 6) | (static_cast<std::uint32_t>(p) << 12) | (move_type << 16);
}

constexpr std::uint8_t  get_from_square(std::uint32_t move) noexcept { return move & 0x3f; }
constexpr std::uint8_t  get_to_square(std::uint32_t move)   noexcept { return move >> 6 & 0x3f; }
constexpr piece_idx     get_piece_idx(std::uint32_t move)   noexcept { return static_cast<piece_idx>(move >> 12 & 0x0f); }
constexpr std::uint16_t get_move_type(std::uint32_t move)   noexcept { return move >> 16; }

// The move must have zero-initialised fields for these set functions to work. Otherwise, you need to call clear first.
constexpr std::uint32_t set_from_square(std::uint32_t move, std::uint8_t from_square) noexcept { return move | from_square; }
constexpr std::uint32_t set_to_square(std::uint32_t move,   std::uint8_t to_square)   noexcept { return move | to_square << 6; }
constexpr std::uint32_t set_piece_idx(std::uint32_t move,   piece_idx p)              noexcept { return move | static_cast<std::uint32_t>(p) << 12; }
constexpr std::uint32_t set_move_type(std::uint32_t move,   std::uint16_t move_type)  noexcept { return move | move_type << 16; }

constexpr std::uint32_t clear_from_square(std::uint32_t move) noexcept { return move & ~0x3f; }
constexpr std::uint32_t clear_to_square(std::uint32_t move)   noexcept { return move & ~(0x3f << 6); }
constexpr std::uint32_t clear_piece_idx(std::uint32_t move)   noexcept { return move & ~(0x0f << 12); }
constexpr std::uint32_t clear_move_type(std::uint32_t move)   noexcept { return move & ~(0xff00 << 16); }

// We use a bitmasks of various (not necessarily orthagonal) move information to form the move type. The only
// ones technically needed to specify all possible moves from standard chess positions are the pawn-promotion
// details (for Chess960 you also need the castling data). Note that a fully quiet move is one with all zeros.
namespace move_type
{

// Bitmasks.
constexpr std::uint16_t CAPTURE            { 0x0010 };
constexpr std::uint16_t CASTLE             { 0x0020 };
constexpr std::uint16_t PROMOTION          { 0x0040 };
constexpr std::uint16_t SINGLE_PAWN_PUSH   { 0x0080 };
constexpr std::uint16_t DOUBLE_PAWN_PUSH   { 0x0100 };
constexpr std::uint16_t EN_PASSENT_CAPTURE { 0x0200 };

// Castles.
constexpr std::uint16_t CASTLE_KS { 0x0001 };
constexpr std::uint16_t CASTLE_QS { 0x0002 };

// Promotions.
constexpr std::uint16_t PROMOTION_KNIGHT { 0x0001 };
constexpr std::uint16_t PROMOTION_BISHOP { 0x0002 };
constexpr std::uint16_t PROMOTION_ROOK   { 0x0004 };
constexpr std::uint16_t PROMOTION_QUEEN  { 0x0008 };

// Additional bitmask meta (rarely used).
constexpr std::uint16_t KNOWN_CHECK     { 0x1000 };
constexpr std::uint16_t KNOWN_NOT_CHECK { 0x2000 };
constexpr std::uint16_t REPETITION      { 0x4000 };

}

// A variant of algebraic long notation that is used by UCI.
std::string to_algebraic_long(std::uint32_t move);

}