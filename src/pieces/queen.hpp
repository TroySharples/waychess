#pragma once

#include "bitboard.hpp"

bitboard get_queen_xrayed_squares_from_mailbox(mailbox x) noexcept;
bitboard get_queen_blocker_squares_from_mailbox(mailbox x) noexcept;
bitboard get_queen_attacked_squares_from_mailbox(mailbox x, bitboard pos) noexcept;