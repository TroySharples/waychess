#pragma once

#include "bitboard.hpp"

bitboard get_rook_xrayed_squares_from_mailbox(mailbox x) noexcept;
bitboard get_rook_blocker_squares_from_mailbox(mailbox x) noexcept;
bitboard get_rook_attacked_squares_from_mailbox(mailbox x, bitboard pos) noexcept;