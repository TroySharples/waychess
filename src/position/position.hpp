#pragma once

#include "bitboard.hpp"

struct position
{
    bool white_to_move;

    bitboard white_pieces;
    bitboard black_pieces;

    bitboard pawns;
    bitboard knights;
    bitboard kings;
    bitboard rooks;
    bitboard queens;
    bitboard bishops;

    bitboard en_passent_piece;
    bitboard en_passent_square;

    bool white_kingside_castle_rights;
    bool white_queenside_castle_rights;

    bool black_kingside_castle_rights;
    bool black_queenside_castle_rights;

    friend std::ostream& operator<<(std::ostream& os, const position& v);
};

constexpr position STARTING_POSITION {
    .white_to_move                 = true,
    .white_pieces                  = RANK_1 | RANK_2,
    .black_pieces                  = RANK_7 | RANK_8,
    .pawns                         = RANK_2 | RANK_7,
    .knights                       = (1ULL << 1) | (1ULL << 6) | (1ULL << 57) | (1ULL << 62),
    .kings                         = (1ULL << 4) | (1ULL << 60),
    .rooks                         = (1Ull << 0) | (1ULL << 7) | (1ULL << 56) | (1ULL << 63),
    .queens                        = (1ULL << 3) | (1ULL << 59),
    .bishops                       = (1ULL << 2) | (1ULL << 5) | (1ULL << 58) | (1ULL << 61),
    .en_passent_square             = 0,
    .white_kingside_castle_rights  = true,
    .white_queenside_castle_rights = true,
    .black_kingside_castle_rights  = true,
    .black_queenside_castle_rights = true
};

constexpr bitboard& to_move_pieces(position& pos) noexcept { return pos.white_to_move ? pos.white_pieces : pos.black_pieces; }
constexpr bitboard& opponent_pieces(position& pos) noexcept { return pos.white_to_move ? pos.black_pieces : pos.white_pieces; }

constexpr const bitboard& to_move_pieces(const position& pos)  noexcept { return pos.white_to_move ? pos.white_pieces : pos.black_pieces; }
constexpr const bitboard& opponent_pieces(const position& pos) noexcept { return pos.white_to_move ? pos.black_pieces : pos.black_pieces; }

std::vector<position> get_next_positions(position pos);