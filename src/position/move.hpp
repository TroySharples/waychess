#pragma once

#include "pieces/pieces.hpp"

struct bitboard;

// Moves are serialised as 32-bit integers - 16 bits to encode the mechanics of a move, and
// an additional 16 bits to contain meta-data about the move type. We also encode a minimal
// set of information needed to unmake the move if needed. This basically boils down to
// castling rights, en-passent squares in the position, and the previous 50-ply rule count.
//      0 -  5 : from-square mb
//      6 - 11 : to-square mb
//     12 - 14 : piece idx
//     15 - 17 : move-type bitmask
//     18 - 20 : move-info enum
//     21 - 24 : unmake-castling
//     25 - 31 : unmake-en-passent / 50m-ply

namespace move
{

// We use a bitmasks of various (not necessarily orthogonal) move information to form the move type.
namespace move_type
{

constexpr std::uint8_t CAPTURE       { 0b100 };
constexpr std::uint8_t CASTLE        { 0b001 };
constexpr std::uint8_t PROMOTION     { 0b011 };
constexpr std::uint8_t PAWN_PUSH     { 0b010 };
constexpr std::uint8_t EN_PASSENT    { 0b110 };

constexpr bool is_castle(std::uint8_t move_type)        noexcept { return move_type == 0b001; }
constexpr bool is_capture(std::uint8_t move_type)       noexcept { return move_type & 0b100; }
constexpr bool is_promotion(std::uint8_t move_type)     noexcept { return (move_type & 0b011) == 0b011; }
constexpr bool is_en_passent(std::uint8_t move_type)    noexcept { return move_type == 0b110; }
constexpr bool is_pawn_push(std::uint8_t move_type)     noexcept { return move_type == 0b010; }

}

// We use a variety of enums to signal additional information about the move. As we only have three bits
// to play with, the move-type is needed to switch how we interpret this.
namespace move_info
{

// Castling.
constexpr std::uint8_t CASTLE_KS { 0b00 };
constexpr std::uint8_t CASTLE_QS { 0b01 };

// First three bits of piece ID for unmake captures and pawn promotions. The remaining bit is determined
// by the side to play.

// Pawn push.
constexpr std::uint8_t PAWN_PUSH_SINGLE { 0b00 };
constexpr std::uint8_t PAWN_PUSH_DOUBLE { 0b01 };

}

// We have 7 bits to encode the unmake-en-passent + unmake-50m-ply. The trick here
// is that if the previous position contained an en-passent square, we know that
// the 50m-ply is zero. If the previous position contained an en-passent square,
// we encode this four 1-bits with the least significant 3 bits of the mailbox
// representation of the en-passent square (whether it is on the black or white
// side is recoverable from the side-to-play). Otherwise, we just encode the
// 50m-ply. This is unambiguous, as the maximum value of the 50m-ply is 100, which
// is less than 0b1111000 = 112 (an en-passent square on either a3 ot a6).

constexpr std::uint32_t serialise_from_mb(std::uint8_t mb)                      noexcept { return mb; } 
constexpr std::uint32_t serialise_to_mb(std::uint8_t mb)                        noexcept { return mb << 6; }
constexpr std::uint32_t serialise_piece_idx(piece_idx p)                        noexcept { return static_cast<std::uint32_t>(p & 0x07) << 12; }
constexpr std::uint32_t serialise_move_type(std::uint8_t move_type)             noexcept { return (move_type & 0x07) << 15; }
constexpr std::uint32_t serialise_move_info(std::uint8_t move_info)             noexcept { return (move_info & 0x07) << 18; }
constexpr std::uint32_t serialise_unmake_castling(std::uint8_t castling)        noexcept { return (castling  & 0x0f) << 21; }
constexpr std::uint32_t serialise_unmake_en_passent(std::uint8_t en_passent_mb) noexcept { return (0x78 | en_passent_mb) << 25; }
constexpr std::uint32_t serialise_unmake_ply_50m(std::uint8_t ply_50m)          noexcept { return ply_50m << 25; }

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
// the movement of the piece in reverse - for example, reverse white pawn push would still
// be from e2 to e4.
constexpr std::uint32_t serialise_unmake(std::uint8_t from_square, std::uint8_t to_square, piece_idx p, std::uint8_t move_type = 0, std::uint8_t move_info = 0, std::uint8_t castling = 0, std::uint8_t en_passent_bb = 0, std::uint8_t ply_50m = 0)
{
    return serialise_from_mb(from_square)
         | serialise_to_mb(to_square)
         | serialise_piece_idx(p)
         | serialise_move_type(move_type)
         | serialise_move_info(move_info)
         | serialise_unmake_castling(castling)
         | (en_passent_bb ? serialise_unmake_en_passent(en_passent_bb) : serialise_unmake_ply_50m(ply_50m));
}

// For the en-passent square, you will have to or it with 0b010000 if it is on the black side.

constexpr std::uint8_t deserialise_from_mb(std::uint32_t move)           noexcept { return move & 0x3f; }
constexpr std::uint8_t deserialise_to_mb(std::uint32_t move)             noexcept { return move >> 6 & 0x3f; }
constexpr piece_idx    deserialise_piece_idx(std::uint32_t move)         noexcept { return static_cast<piece_idx>(move >> 12 & 0x07); }
constexpr std::uint8_t deserialise_move_type(std::uint32_t move)         noexcept { return move >> 15 & 0x07; }
constexpr std::uint8_t deserialise_move_info(std::uint32_t move)         noexcept { return move >> 18 & 0x07; }
constexpr std::uint8_t deserialise_unmake_castling(std::uint32_t move)   noexcept { return move >> 21 & 0x0f; }
constexpr std::uint8_t deserialise_unmake_en_passent(std::uint32_t move) noexcept { return move >> 25 & 0x07; }
constexpr std::uint8_t deserialise_unmake_ply_50m(std::uint32_t move)    noexcept { return move >> 25 & 0x7f; }

// A variant of algebraic long notation that is used by UCI.
std::string to_algebraic_long(std::uint32_t move) noexcept;

// Very slow implementation of long-notation to our internal move representation. This needs the bitboard the move
// is made from to fill in all the additional meta fields. This works by generating all the legal moves, and then
// doing a string-compare on each one!
std::uint32_t from_algebraic_long(std::string_view algebraic, const bitboard& bb);

// Validates that the string does indeed encode a feasible (possibly illegal) move in long-notation.
bool is_algebraic_long(std::string_view algebraic);

}