#pragma once

#include "details/prn_codes.hpp"
#include "position/mailbox.hpp"

namespace zobrist
{

constexpr std::uint64_t get_code_piece(piece_idx id, std::uint8_t mb) noexcept
{
    return details::PRN_CODE_ARRAY[(id << 6) | mb];
}

constexpr std::uint64_t get_code_en_passent(std::uint8_t en_passent_mb) noexcept
{
    return get_code_piece(piece_idx::w_pawn, en_passent_mb & 007);
}

constexpr std::uint64_t get_code_castling(std::uint8_t castling)
{
    return get_code_piece(piece_idx::w_any, castling);
}

constexpr std::uint64_t CODE_IS_BLACK_TO_MOVE { get_code_piece(piece_idx::w_any, 0x10) };

// This should only be called when initialising the hash from a given position.
// It should be the responsibility of make/unmake move to increment the hash.
constexpr std::uint64_t hash_init(const mailbox& mb) noexcept
{
    std::uint64_t ret {};

    // Fill out the piece codes.
    for (std::size_t i = 0; i < mb.squares.size(); i++)
        ret ^= get_code_piece(mb.squares[i], i);

    // Fill out en-passent.
    if (mb.en_passent_square.has_value())
        ret ^= get_code_en_passent((*mb.en_passent_square) & 007);

    // Fill out side-to-move.
    if (mb.is_black_to_play())
        ret ^= CODE_IS_BLACK_TO_MOVE;

    // Fill out castling rights.
    ret ^= get_code_castling(mb.castling);

    return ret;
}

}