#include "make_move.hpp"

#include "move.hpp"
#include "pieces/pieces.hpp"

void make_move(bitboard& bb, std::uint32_t move)
{
    const bool is_black_to_play { bb.is_black_to_play() };
    std::uint64_t& to_move_pieces  { is_black_to_play ? bb.b_pieces : bb.w_pieces };

    const std::uint8_t from_square { get_from_square(move) };
    const std::uint8_t to_square { get_to_square(move) };
    const piece_idx p { get_piece_idx(move) };
    const std::uint16_t move_type { get_move_type(move) };

    const std::uint64_t from_bitboard { 1ULL << from_square };
    const std::uint64_t to_bitboard   { 1ULL << to_square };
    const std::uint64_t move_bitboard { from_bitboard | to_bitboard };

    // Handle mechanically moving the side-to-plays piece.
    to_move_pieces ^= move_bitboard;
    bb.boards[p] ^= move_bitboard;

    // Handle the case that this move is a capture.
    if (move_type & move_type::CAPTURE)
    {
        std::uint64_t& opponent_pieces { is_black_to_play ? bb.w_pieces : bb.b_pieces };

        opponent_pieces ^= to_bitboard;
        for (std::uint8_t idx = is_black_to_play ? piece_idx::w_pawn : piece_idx::b_pawn; idx <= is_black_to_play ? piece_idx::w_queen : piece_idx::b_queen; idx++)
        {
            if (std::uint64_t& capture_bitboard = bb.boards[idx]; capture_bitboard & to_bitboard)
            {
                capture_bitboard ^= to_bitboard;
                break;
            }
        }
    }

    // For now ignore special moves like en-passent or castling.
}