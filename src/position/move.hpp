#pragma once

#include "pieces/pieces.hpp"

#include <x86intrin.h>

// Moves are serialised as 32-bit integers - 16 bits to encode the mechanics of a move, and
// an additional 16 bits to contain meta-data about the move type. We also encode a minimal
// set of information needed to unmake the move if needed. This basically boils down to
// castling rights, en-passent squares in the position, and the previous 50-ply rule count.
//      0 -  5 : from-square mb
//      6 - 11 : to-square mb
//     12 - 14 : piece idx
//     15 - 17 : move-type bitmask
//     18 - 19 : move-info enum
//     20 - 23 : unmake-castling
//     24 - 31 : unmake-en-passent / 50m-ply

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

// We have 8 bits to encode the unmake-en-passent + unmake-50m-ply. The trick here
// is that if the previous position contained an en-passent square, we know that
// the 50m-ply is zero. We first use bit 0 to represent whether there are any
// en-passent bits. If there are, the subsequent 7 bits represent the mailbox
// en-passent square. Otherwise, the 7 bits instead contain the 50m-ply (up to a
// maximum of 100 ply).

constexpr std::uint32_t serialise_from_mb(std::uint8_t mb)                       noexcept { return mb; } 
constexpr std::uint32_t serialise_to_mb(std::uint8_t mb)                         noexcept { return mb << 6; }
constexpr std::uint32_t serialise_piece_idx(piece_idx p)                         noexcept { return static_cast<std::uint32_t>(p & 0x07) << 12; }
constexpr std::uint32_t serialise_move_type(std::uint8_t move_type)              noexcept { return move_type << 15; }
constexpr std::uint32_t serialise_move_info(std::uint8_t move_info)              noexcept { return move_info << 18; }
constexpr std::uint32_t serialise_unmake_castling(std::uint8_t castling)         noexcept { return castling << 20; }
constexpr std::uint32_t serialise_unmake_en_passent(std::uint64_t en_passent_bb) noexcept { return (std::countr_zero(en_passent_bb)) << 24; }
constexpr std::uint32_t serialise_unmake_ply_50m(std::uint8_t ply_50m)           noexcept { return ply_50m << 24; }
constexpr std::uint32_t serialise_unmake_switch(bool is_en_passent)              noexcept { return is_en_passent ? 0x80000000 : 0x00000000; }

// For serialising moves designed to iterate forwards.
constexpr std::uint32_t serialise_make(std::uint8_t from_square, std::uint8_t to_square, piece_idx p, std::uint8_t move_type = 0, std::uint8_t move_info = 0)
{
    return serialise_from_mb(from_square)
         | serialise_to_mb(to_square)
         | serialise_piece_idx(p)
         | serialise_move_type(move_type)
         | serialise_move_info(move_info);
}

// For serialising moved designed to iterate backwards. The from/to notation describes
// the movement of the piece in reverse - for example, reverse white pawn push could be
// from e4 to e2.
constexpr std::uint32_t serialise_unmake(std::uint8_t from_square, std::uint8_t to_square, piece_idx p, std::uint8_t move_type = 0, std::uint8_t move_info = 0, std::uint8_t castling = 0, std::uint8_t en_passent_bb = 0, std::uint8_t ply_50m = 0)
{
    return serialise_from_mb(from_square)
         | serialise_to_mb(to_square)
         | serialise_piece_idx(p)
         | serialise_move_type(move_type)
         | serialise_move_info(move_info)
         | serialise_unmake_castling(castling)
         | serialise_unmake_en_passent(en_passent_bb)
         | serialise_unmake_ply_50m(ply_50m)
         | serialise_unmake_switch(en_passent_bb);
}


constexpr std::uint8_t  deserialise_from_mb(std::uint32_t move)           noexcept { return move & 0x3f; }
constexpr std::uint8_t  deserialise_to_mb(std::uint32_t move)             noexcept { return move >> 6 & 0x3f; }
constexpr piece_idx     deserialise_piece_idx(std::uint32_t move)         noexcept { return static_cast<piece_idx>(move >> 12 & 0x07); }
constexpr std::uint8_t  deserialise_move_type(std::uint32_t move)         noexcept { return move >> 15 & 0x07; }
constexpr std::uint8_t  deserialise_move_info(std::uint32_t move)         noexcept { return move >> 18 & 0x03; }
constexpr std::uint8_t  deserialise_unmake_castling(std::uint32_t move)   noexcept { return move >> 20 & 0x0f; }
constexpr std::uint64_t deserialise_unmake_en_passent(std::uint32_t move) noexcept { return 1ULL << (move >> 24 & 0x7f); }
constexpr std::uint8_t  deserialise_unmake_ply_50m(std::uint32_t move)    noexcept { return move >> 24 & 0x7f; }
constexpr bool          deserialise_unmake_switch(std::uint32_t move)     noexcept { return move >> 31; }

// A variant of algebraic long notation that is used by UCI.
std::string to_algebraic_long(std::uint32_t move);

}