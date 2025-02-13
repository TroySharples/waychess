#pragma once

#include "board.hpp"

bitboard get_knight_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_knight_attacked_squares_from_mailbox(mailbox b) noexcept;