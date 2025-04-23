#pragma once

#include "bitboard.hpp"

bitboard get_king_attacked_squares_from_bitboard(bitboard b) noexcept;
bitboard get_king_attacked_squares_from_mailbox(mailbox b) noexcept;