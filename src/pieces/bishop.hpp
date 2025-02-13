#pragma once

#include "board.hpp"

bitboard get_bishop_xrayed_squares_from_mailbox(mailbox x) noexcept;
bitboard get_bishop_blocker_squares_from_mailbox(mailbox x) noexcept;
bitboard get_bishop_attacked_squares_from_mailbox(mailbox x, bitboard pos) noexcept;
