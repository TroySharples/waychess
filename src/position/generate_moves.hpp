#pragma once

#include "bitboard.hpp"

// The maximum number of legal moves in any one position (I think one with lots of queens everywhere). In reality
// it's usually about 20, but this constant is important for working out how much RAM to allocate.
constexpr std::size_t MAX_MOVES_PER_POSITION { 218 };

// Generates all legal moves, as well as moves that would be legal were it not for "checking" rules (i.e. the
// king is left in check after the move is played, the king is castling out of check, or the king is castling
// into check). We take in a span to hold our moves, and return a subspan containing the actual moves. Note
// that the input span must be large enough to hold all generated moves.
inline std::size_t generate_pseudo_legal_moves(const bitboard& bb, std::span<std::uint32_t> move_buf) noexcept;

#include "details/generate_moves.hpp"