#pragma once

#include "bitboard.hpp"

// Generates all legal moves, as well as moves that would be legal were it not for "checking" rules (i.e. the
// king is left in check after the move is played, the king is castling out of check, or the king is castling
// into check).
std::vector<std::uint32_t> generate_pseudo_legal_moves(const bitboard& bb);