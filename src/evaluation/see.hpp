#pragma once

#include "pieces/pieces.hpp"
#include "position/attacks.hpp"
#include "position/move.hpp"
#include "evaluation/evaluate_material.hpp"
#include "utility/binary.hpp"

namespace evaluation
{

// Uses the iterative SEE swap-algorithm.
inline int see_capture(const bitboard& bb, std::uint32_t move, bool is_black)
{
    const std::size_t from_mb { move::make_decode_from_mb(move) };
    const std::size_t to_mb   { move::make_decode_to_mb(move)   };

    const std::uint64_t to_bb { 1ULL << to_mb };

    // These values will be updated as the algorithm iterates.
    std::uint64_t from_bb  { 1ULL << from_mb };
    piece_idx victim_idx   { bb.get_piece_type_colour(to_bb, !is_black) };
    piece_idx attacker_idx { move::make_decode_piece_idx(move) };

    // The occupancy bitboard of all pieces.
    std::uint64_t occ_bb { bb.boards[piece_idx::w_any] | bb.boards[piece_idx::b_any] };

    // Bitboard of all pieces (both colours) attacking our square of interest.
    std::uint64_t attackers_bb { get_attackers(bb, to_mb) };

    // Bitboards of the xrays of two-types emanating from the square of interest.
    const std::uint64_t rook_xrays_bb   { get_rook_xrayed_squares_from_mailbox(to_mb) };
    const std::uint64_t bishop_xrays_bb { get_bishop_xrayed_squares_from_mailbox(to_mb) };

    // Bitboard of xraying pieces of two types, that aren't directly attacking the square of interest but may later
    // do if the piece in front of it moves.
    std::uint64_t rook_xrayers_bb   { rook_xrays_bb   & ~attackers_bb & (bb.boards[piece_idx::w_rook]   | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_rook]   | bb.boards[piece_idx::b_queen]) };
    std::uint64_t bishop_xrayers_bb { bishop_xrays_bb & ~attackers_bb & (bb.boards[piece_idx::w_bishop] | bb.boards[piece_idx::w_queen] | bb.boards[piece_idx::b_bishop] | bb.boards[piece_idx::b_queen]) };

    // A gain array to keep track of how much value is in each capture - we will recurse backwards over this at the end
    // to allow for the attacking side to "not" capture if needed.
    std::size_t d = 0;
    std::array<int, 32> gain;

    gain[d] = std::abs(evaluation::piece_mg_evaluation[victim_idx]);
    do
    {
        // Compute the differential gain.
        d++;
        gain[d] = std::abs(evaluation::piece_mg_evaluation[attacker_idx]) - gain[d-1];

        // Remove this attacker from the list and reset the occupancy (for xrays).
        attackers_bb ^= from_bb;
        occ_bb       ^= from_bb;

        // Update xrayers. We do this by checking if the capturing piece was on the xray of any xraying piece, in which case moving it
        // may have cleared the path for the unmasked piece to make the capture. We also make use of the fact that any capture can
        // unmask at most one other attacking piece. We have to remember to unset this xrayer after. I don't believe it is actually
        // necessary to take special care over pieces that are both rook-xrayers and bishop-xrayers (i.e. queens) because they can't
        // act both ways at once towards a specific square - a queen is either attacking a square as a rook or bishop.
        if (from_bb & rook_xrays_bb)
        {
            if (const std::uint64_t unmasked_rook_attacker { rook_xrayers_bb & get_rook_attacked_squares_from_mailbox(occ_bb, to_mb) }) [[unlikely]]
            {
                attackers_bb    |= unmasked_rook_attacker;
                rook_xrayers_bb ^= unmasked_rook_attacker;
            }
        }
        if (from_bb & bishop_xrays_bb)
        {
            if (const std::uint64_t unmasked_bishop_attacker { bishop_xrayers_bb & get_bishop_attacked_squares_from_mailbox(occ_bb, to_mb) }) [[unlikely]]
            {
                attackers_bb      |= unmasked_bishop_attacker;
                bishop_xrayers_bb ^= unmasked_bishop_attacker;
            }
        }

        is_black = !is_black;
        {
            const auto v = bb.get_least_valuable_piece(attackers_bb, is_black);
            attacker_idx = v.first;
            from_bb = ls1b_isolate(v.second);
        }
    }
    while (from_bb);

    // Iterate backwards negamax-style to work out the final SEE.
    while (--d)
        gain[d-1] = -std::max(-gain[d-1], gain[d]);

    return gain[0];
}

}