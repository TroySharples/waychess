#pragma once

#include "pieces/pieces.hpp"

#include <x86intrin.h>

// Moves are serialised as 32-bit integers - 16 bits to encode the mechanics of a move, and
// an additional 16 bits to contain meta-data about the move type. We also encode a minimal
// set of information needed to unmake the move if needed. This basically boils down to
// castling rights and en-passent squares in the position.
//      0 -  5 : from-square mb
//      6 - 11 : to-square mb
//     12 - 15 : piece idx
//     16 - 18 : move-type bitmask
//     19 - 20 : move-info enum
//     23 - 26 : unmake-castling
//     27 - 31 : unmake-en-passent

namespace move
{

// We use a bitmasks of various (not necessarily orthagonal) move information to form the move type.
namespace move_type
{

constexpr std::uint8_t CAPTURE       { 0b100 };
constexpr std::uint8_t CASTLE        { 0b001 };
constexpr std::uint8_t PROMOTION     { 0b011 };
constexpr std::uint8_t PAWN_PUSH     { 0b010 };
constexpr std::uint8_t EN_PASSENT    { 0b110 };
constexpr std::uint8_t CAPTURES_PAWN { 0b101 };

constexpr bool is_castle(std::uint8_t move_type)        noexcept { return move_type == 0b001; }
constexpr bool is_capture(std::uint8_t move_type)       noexcept { return move_type & 0b100; }
constexpr bool is_promotion(std::uint8_t move_type)     noexcept { return (move_type & 0b011) == 0b011; }
constexpr bool is_en_passent(std::uint8_t move_type)    noexcept { return move_type == 0b110; }
constexpr bool is_pawn_push(std::uint8_t move_type)     noexcept { return move_type == 0b010; }
constexpr bool is_captures_pawn(std::uint8_t move_type) noexcept { return move_type == 0b101; }

}

// We use a variety of enums to signal additional information about the move. As we only have two bits
// to play with, the move-type is needed to switch how we interpret this.
namespace move_info
{

// Castling.
constexpr std::uint8_t CASTLE_KS { 0b00 };
constexpr std::uint8_t CASTLE_QS { 0b01 };

// Pawn promotion.
constexpr std::uint8_t PROMOTION_KNIGHT { 0b00 };
constexpr std::uint8_t PROMOTION_BISHOP { 0b01 };
constexpr std::uint8_t PROMOTION_ROOK   { 0b10 };
constexpr std::uint8_t PROMOTION_QUEEN  { 0b11 };

// Pawn push.
constexpr std::uint8_t PAWN_PUSH_SINGLE { 0b00 };
constexpr std::uint8_t PAWN_PUSH_DOUBLE { 0b01 };

}

// We have 5 bits to encode the unmake-en-passent. We use the bit 0 to represent
// whether there are any en-passent bits, bit 1 to represent whether it is on the
// black or white side of the board, and bits 2-4 to represent which respective
// square it is.

constexpr std::uint32_t serialise_from_mb(std::uint8_t mb)                    noexcept { return mb; } 
constexpr std::uint32_t serialise_to_mb(std::uint8_t mb)                      noexcept { return mb << 6; }
constexpr std::uint32_t serialise_piece_idx(piece_idx p)                      noexcept { return static_cast<std::uint32_t>(p) << 12; }
constexpr std::uint32_t serialise_move_type(std::uint8_t move_type)           noexcept { return move_type << 16; }
constexpr std::uint32_t serialise_move_info(std::uint8_t move_info)           noexcept { return move_info << 19; }
constexpr std::uint32_t serialise_unmake_castling(std::uint8_t castling)      noexcept { return castling << 23; }
constexpr std::uint32_t serialise_unmake_en_passent(std::uint64_t en_passent) noexcept { return en_passent ? (_pext_u32(std::countr_zero(en_passent), 0b100111) | 0x10) << 27 : 0; }

constexpr std::uint32_t serialise(std::uint8_t from_square, std::uint8_t to_square, piece_idx p, std::uint8_t move_type = 0, std::uint8_t move_info = 0, std::uint8_t castling = 0, std::uint64_t en_passent = 0)
{
    return serialise_from_mb(from_square)
         | serialise_to_mb(to_square)
         | serialise_piece_idx(p)
         | serialise_move_type(move_type)
         | serialise_move_info(move_info)
         | serialise_unmake_castling(castling)
         | serialise_unmake_en_passent(en_passent);
}

constexpr std::uint8_t  deserialise_from_mb(std::uint32_t move)           noexcept { return move & 0x3f; }
constexpr std::uint8_t  deserialise_to_mb(std::uint32_t move)             noexcept { return move >> 6 & 0x3f; }
constexpr piece_idx     deserialise_piece_idx(std::uint32_t move)         noexcept { return static_cast<piece_idx>(move >> 12 & 0x0f); }
constexpr std::uint8_t  deserialise_move_type(std::uint32_t move)         noexcept { return move >> 16 & 0x07; }
constexpr std::uint8_t  deserialise_move_info(std::uint32_t move)         noexcept { return move >> 19 & 0x03; }
constexpr std::uint8_t  deserialise_unmake_castling(std::uint32_t move)   noexcept { return move >> 23 & 0x0f; }
constexpr std::uint64_t deserialise_unmake_en_passent(std::uint32_t move) noexcept { return move & 0x80000000 ? 1ULL << (_pdep_u32(move >> 27 & 0x1f, 0b100111) | 0b001000) : 0; }

// A variant of algebraic long notation that is used by UCI.
std::string to_algebraic_long(std::uint32_t move);

}