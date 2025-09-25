#pragma once

#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"
#include "utility/binary.hpp"
#include <immintrin.h>

struct bitboard;

namespace move
{

// ############################################################################################
// INTRODUCTION
//
// Moves are serialised as 32-bit integers. Often, this will be stored in the lower part of a
// wider 64-bit integer, with the upper bits containing some meta-information about the move
// (e.g. score for alpha/beta ordering).
//
// To unmake a move, you need a bit of extra state regarding bits of the position that might
// have been lost in the making of the move (e.g. castling rights, en-passent squares). This is
// stored in it's own 32-bit integer, that is filled-out at the time of making the move.
// ############################################################################################

// ############################################################################################
// MAKE
//
// The 32-bit integer that contains all the information needed to increment the position:
//      0 -  5 : from-square mb
//      6 - 11 : to-square mb
//     12 - 15 : piece idx
//     16 - 19 : move-promotion idx
//     20 - 27 : move-type bitmask
//     28 - 31 : move-info bitmask
//
// The move-info itself is not part of the definition of the move, but is filled out later by
// a move-scoring system that includes interesting information about the move. These bits
// should be masked off when comparing moves.
// ############################################################################################

// The null move is an (illegal) chess move to just do nothing and pass the turn to the other
// side. The make-move function simply increments the side-to-play and the ply counter.
constexpr std::uint32_t NULL_MOVE { 0 };

// We use a bitmasks of various (not necessarily orthogonal) move information to form the move type.
namespace type
{

namespace details { constexpr std::size_t OFFSET { 20 }; }

constexpr std::uint32_t CAPTURE          { 0b00000001U << details::OFFSET };
constexpr std::uint32_t CASTLE_KS        { 0b00000010U << details::OFFSET };
constexpr std::uint32_t CASTLE_QS        { 0b00000100U << details::OFFSET };
constexpr std::uint32_t PROMOTION        { 0b00001000U << details::OFFSET };
constexpr std::uint32_t PAWN_PUSH_SINGLE { 0b00010000U << details::OFFSET };
constexpr std::uint32_t PAWN_PUSH_DOUBLE { 0b00100000U << details::OFFSET };
constexpr std::uint32_t EN_PASSENT       { 0b01000000U << details::OFFSET };

constexpr std::uint32_t zeroise(std::uint32_t move) noexcept { return move &= ~(0xff << details::OFFSET); }

}

namespace info
{

namespace details { constexpr std::size_t OFFSET { 28 }; }

constexpr std::uint32_t CHECK  { 0b0001U << details::OFFSET };
constexpr std::uint32_t KILLER { 0b0010U << details::OFFSET };
constexpr std::uint32_t PV     { 0b0100U << details::OFFSET };
constexpr std::uint32_t HASH   { 0b1000U << details::OFFSET };

}

constexpr bool move_is_equal(std::uint32_t a, std::uint32_t b) noexcept { return (0x0fffffff & a) == (0x0fffffff & b); }

constexpr std::uint32_t make_encode_from_mb(std::size_t mb) noexcept { return 0x3f & mb; } 
constexpr std::uint32_t make_encode_to_mb(std::size_t mb)   noexcept { return (0x3f & mb) << 6; }
constexpr std::uint32_t make_encode_piece_idx(piece_idx p)  noexcept { return static_cast<std::uint32_t>(0x0f & p) << 12; }
constexpr std::uint32_t make_encode_promotion(piece_idx p)  noexcept { return static_cast<std::uint32_t>(0x0f & p) << 16; }

constexpr std::uint32_t make_encode(std::size_t from_mb, std::size_t to_mb, piece_idx to_move, piece_idx promotion = static_cast<piece_idx>(0))
{
    return make_encode_from_mb(from_mb)
         | make_encode_to_mb(to_mb)
         | make_encode_piece_idx(to_move)
         | make_encode_promotion(promotion);
}

constexpr std::size_t make_decode_from_mb(std::uint32_t move)   noexcept { return move & 0x3f; }
constexpr std::size_t make_decode_to_mb(std::uint32_t move)     noexcept { return (move >> 6) & 0x3f; }
constexpr piece_idx   make_decode_piece_idx(std::uint32_t move) noexcept { return static_cast<piece_idx>((move >> 12) & 0x0f); }
constexpr piece_idx   make_decode_promotion(std::uint32_t move) noexcept { return static_cast<piece_idx>((move >> 16) & 0x0f); }

// ############################################################################################
// UNMAKE
//
// The 32-bit integer that contains the additional information (as well as the 32-bit make) to
// unmake a move to a position:
//      0 -  3 : castling
//      4 -  7 : capture idx
//      8 - 15 : 50-move ply counter
//     16 - 31 : en-passent square (pext-reduced)
// ############################################################################################

inline std::uint32_t unmake_encode_castling(std::uint8_t castling) noexcept { return static_cast<std::uint32_t>(0x0f & castling); }
inline std::uint32_t unmake_encode_capture(piece_idx capture)      noexcept { return static_cast<std::uint32_t>(0x0f & capture)  << 4; }
inline std::uint32_t unmake_encode_ply_50m(std::uint8_t ply_50m)   noexcept { return static_cast<std::uint32_t>(0xff & ply_50m)  << 8; }
inline std::uint32_t unmake_encode_en_passent(std::uint64_t ep_bb) noexcept { return _pext_u64(ep_bb, RANK_3 | RANK_6)           << 16; }

inline std::uint32_t unmake_encode(std::uint8_t castling, piece_idx capture, std::uint8_t ply_50m, std::uint64_t ep_bb)
{
    return unmake_encode_castling(castling)
         | unmake_encode_capture(capture)
         | unmake_encode_ply_50m(ply_50m)
         | unmake_encode_en_passent(ep_bb);
}

inline std::uint8_t  unmake_decode_castling(std::uint32_t unmake)   noexcept { return unmake & 0x0f; }
inline piece_idx     unmake_decode_capture(std::uint32_t unmake)    noexcept { return static_cast<piece_idx>((unmake >> 4) & 0x0f); }
inline std::uint8_t  unmake_decode_ply_50m(std::uint32_t unmake)    noexcept { return static_cast<std::uint8_t>((unmake >> 8) & 0xff); }
inline std::uint64_t unmake_decode_en_passent(std::uint32_t unmake) noexcept { return _pdep_u64(unmake >> 16, RANK_3 | RANK_6); }

// ############################################################################################
// HELPER FUNCTIONS
// ############################################################################################

// A variant of algebraic long notation that is used by UCI. We also provide an overload that serialises a variation
// of moves.
std::string to_algebraic_long(std::uint32_t move) noexcept;

template <typename T>
inline std::string to_algebraic_long(std::span<const T> moves) noexcept
{
    std::string ret;

    // Catch the edge-case where our buffer is zero-length.
    if (moves.empty())
        return ret;

    for (const auto move : moves)
    {
        ret.append(to_algebraic_long(move));
        ret.push_back(' ');
    }
    ret.pop_back();

    return ret;
}

// Very slow implementation of long-notation to our internal move representation. This needs the bitboard the move
// is made from to fill in all the additional meta fields. This works by generating all the legal moves, and then
// doing a string-compare on each one!
std::uint32_t from_algebraic_long(std::string_view algebraic, const bitboard& bb);

// Validates that the string does indeed encode a feasible (possibly illegal) move in long-notation.
bool is_algebraic_long(std::string_view algebraic);

}