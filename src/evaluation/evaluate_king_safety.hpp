#pragma once

#include "pieces/pieces.hpp"
#include "position/bitboard.hpp"
#include "utility/binary.hpp"

#include <array>
#include <bit>

namespace evaluation
{

namespace details
{

constexpr std::array<std::uint64_t, 64> white_king_shield_squares_lut { [] () consteval {
    std::array<std::uint64_t, 64> ret {};

    for (std::size_t mb = 0; mb < 64; mb++)
    {
        std::uint64_t shield {};

        // Calculate the (maximum 3) squares right in front of the king.
        const std::uint64_t bb { 1ULL << mb };
        shield |= shift_north_east(bb) | shift_north(bb) | shift_north_west(bb);

        // Shift the rectangle upwards.
        shield |= (shield << 8);


        ret[mb] = shield;
    }

    return ret;
} () };

constexpr std::array<std::uint64_t, 64> black_king_shield_squares_lut { [] () consteval {
    std::array<std::uint64_t, 64> ret {};

    for (std::size_t mb = 0; mb < 64; mb++)
    {
        std::uint64_t shield {};

        // Calculate the (maximum 3) squares right in front of the king.
        const std::uint64_t bb { 1ULL << mb };
        shield |= shift_south_east(bb) | shift_south(bb) | shift_south_west(bb);

        // Shift the rectangle upwards.
        shield |= (shield >> 8);


        ret[mb] = shield;
    }

    return ret;
} () };

constexpr std::array<int, 7> pawn_shield_number_evaluation_mg { -40, -25, -10, 0, 5, 5, 5 };
constexpr std::array<int, 7> pawn_shield_number_evaluation_eg { -5, -2, 0, 0, 1, 1,1 };

constexpr int pawn_shield_hole_mg { -35 };
constexpr int pawn_shield_hole_eg { -2 };

constexpr int king_mobility_penalty_mg { -2 };
constexpr int king_mobility_penalty_eg { 0 };

inline int evaluate_king_safety_generic(const bitboard& bb, bool is_black, bool is_eg) noexcept
{
    int ret {};

    const std::uint64_t king_bb { bb.boards[is_black ? piece_idx::b_king : piece_idx::w_king] };
    const std::uint64_t pawn_bb { bb.boards[is_black ? piece_idx::b_pawn : piece_idx::w_pawn] };

    const std::size_t king_mb       { static_cast<std::size_t>(std::countr_zero(king_bb)) };
    const std::uint64_t shield      { pawn_bb & (is_black ? black_king_shield_squares_lut[king_mb] : white_king_shield_squares_lut[king_mb]) };
    const std::size_t shield_number { static_cast<std::size_t>(std::popcount(shield)) };

    // Give score for how many pawns in shield (note that it's technically possible to have 6 if the king moves up the
    // board, but in practice this is pretty unlikely).
    ret += (is_eg ? pawn_shield_number_evaluation_eg[shield_number] : pawn_shield_number_evaluation_mg[shield_number]);

    // Has the king got a pawn in front of him (we tried originally using the number of holes in the pawn shield, but that didn't
    // work out overly well).
    if (get_bitboard_file(king_mb & 0x07) & pawn_bb)
        ret += (is_eg ? pawn_shield_hole_eg : pawn_shield_hole_mg);

    // Give negative score for king mobility.
    const int mobility { std::popcount(get_queen_attacked_squares_from_mailbox(bb.boards[is_black ? piece_idx::b_any : piece_idx::w_any], king_mb)) };
    ret += (is_eg ? king_mobility_penalty_eg*mobility : king_mobility_penalty_mg*mobility);

    return ret;
}

}

// Returns the set of (maximum 6) squares in front of the king. For example, a white king on g1 would
// have a rectangular shield spanning from f2 to h3.
inline std::uint64_t get_white_king_shield_squares(std::size_t mb) noexcept{ return details::white_king_shield_squares_lut[mb]; }
inline std::uint64_t get_black_king_shield_squares(std::size_t mb) noexcept{ return details::black_king_shield_squares_lut[mb]; }

inline std::uint64_t get_white_pawn_shield(const bitboard& bb) noexcept { return bb.boards[piece_idx::w_pawn] & get_white_king_shield_squares(std::countr_zero(bb.boards[piece_idx::w_king])); }
inline std::uint64_t get_black_pawn_shield(const bitboard& bb) noexcept { return bb.boards[piece_idx::b_pawn] & get_black_king_shield_squares(std::countr_zero(bb.boards[piece_idx::b_king])); }

inline int evaluate_white_king_safety_mg(const bitboard& bb) noexcept { return details::evaluate_king_safety_generic(bb, false, false); }
inline int evaluate_white_king_safety_eg(const bitboard& bb) noexcept { return details::evaluate_king_safety_generic(bb, false, true); }

inline int evaluate_black_king_safety_mg(const bitboard& bb) noexcept { return details::evaluate_king_safety_generic(bb, true, false); }
inline int evaluate_black_king_safety_eg(const bitboard& bb) noexcept { return details::evaluate_king_safety_generic(bb, true, true); }

inline int evaluate_king_safety_mg(const bitboard& bb) noexcept { return evaluate_white_king_safety_mg(bb) - evaluate_black_king_safety_mg(bb); }
inline int evaluate_king_safety_eg(const bitboard& bb) noexcept { return evaluate_white_king_safety_eg(bb) - evaluate_black_king_safety_eg(bb); }


}