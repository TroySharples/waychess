#pragma once

#include "bitboard.hpp"

// Generates all legal moves, as well as moves that would be legal were it not for "checking" rules (i.e. the
// king is left in check after the move is played, the king is castling out of check, or the king is castling
// into check). We take in a span to hold our moves, and return a subspan containing the actual moves. Note
// that the input span must be large enough to hold all generated moves (maximum number legal moves in a chess
// position is 218, so a 256-entry array seems safe...).
inline std::size_t generate_pseudo_legal_moves(const bitboard& bb, std::span<std::uint32_t> move_buf) noexcept;

#include "details/generate_moves.hpp"