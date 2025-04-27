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
constexpr std::uint16_t DOUBLE_PAWN { 0x0010 };
constexpr std::uint16_t CAPTURE     { 0x0020 };
constexpr std::uint16_t CASTLE      { 0x0040 };
constexpr std::uint16_t PROMOTION   { 0x0080 };

// En-passent related moves.
constexpr std::uint16_t DOUBLE_PAWN_PUSH   { DOUBLE_PAWN };
constexpr std::uint16_t EN_PASSENT_CAPTURE { CAPTURE | DOUBLE_PAWN };

// Castles.
constexpr std::uint16_t CASTLE_KS { CASTLE | 0x0001 };
constexpr std::uint16_t CASTLE_QS { CASTLE | 0x0002 };

// Promotions.
constexpr std::uint16_t PROMOTION_KNIGHT         { PROMOTION | 0x0001 };
constexpr std::uint16_t PROMOTION_BISHOP         { PROMOTION | 0x0002 };
constexpr std::uint16_t PROMOTION_ROOK           { PROMOTION | 0x0004 };
constexpr std::uint16_t PROMOTION_QUEEN          { PROMOTION | 0x0008 };
constexpr std::uint16_t PROMOTION_CAPTURE_KNIGHT { CAPTURE | PROMOTION_KNIGHT };
constexpr std::uint16_t PROMOTION_CAPTURE_BISHOP { CAPTURE | PROMOTION_BISHOP };
constexpr std::uint16_t PROMOTION_CAPTURE_ROOK   { CAPTURE | PROMOTION_ROOK };
constexpr std::uint16_t PROMOTION_CAPTURE_QUEEN  { CAPTURE | PROMOTION_QUEEN };

// Additional bitmask meta (rarely used).
constexpr std::uint16_t KNOWN_CHECK     { 0x0100 };
constexpr std::uint16_t KNOWN_NOT_CHECK { 0x0200 };
constexpr std::uint16_t REPETITION      { 0x0400 };

}

// We have an independent move-type test on the serialised move in the hope that the compiler
// might be able to make some optimisation.
constexpr bool is_move_type(std::uint32_t move, std::uint16_t move_type_mask) noexcept { return move & move_type_mask << 16; }