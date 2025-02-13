#pragma once

#include "board.hpp"

bitboard get_bishop_xrayed_squares(mailbox x) noexcept;

bitboard get_bishop_attacked_squares(mailbox x, bitboard pos) noexcept;