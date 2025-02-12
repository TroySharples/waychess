#pragma once

#include "piece.hpp"

bitboard get_rook_xrayed_squares(mailbox x) noexcept;

bitboard get_rook_blocker_squares(mailbox x) noexcept;

extern const bitboard_table xray_table_rook;

bitboard get_rook_attacked_squares(mailbox x, bitboard pos);

extern const sliding_attack_table attack_table_rook;