#include "game_state.hpp"

#include "generate_moves.hpp"
#include "make_move.hpp"
#include "zobrist_hash.hpp"
#include "mailbox.hpp"

game_state::game_state(const bitboard& bb)
    : bb(bb), hash(zobrist::hash_init(mailbox(bb)))
{
    position_history[0] = hash;
}

game_state& game_state::operator=(const bitboard& bb)
{
    this->bb = bb;
    hash = zobrist::hash_init(mailbox(bb));
    position_history[0] = hash;

    return *this;
}

std::span<const std::uint32_t> game_state::get_pv(std::uint8_t ply) const noexcept
{
    // Initialise the return value with the stored PV, possibly containing illegal moves.
    std::span<const std::uint32_t> ret = pv.table[ply];

    // Make a copy of the bitboard and play-out these moves, and escape when we hit an illegal one.
    bitboard bb_copy { bb };

    std::size_t legal_moves {};
    while (legal_moves < ret.size())
    {
        const std::uint32_t move { ret[legal_moves] };

        // Is this move in the list of pseudo-legal moves?
        std::array<std::uint32_t, MAX_MOVES_PER_POSITION> move_buf;
        const std::size_t moves { generate_pseudo_legal_moves(bb_copy, move_buf) };
        const std::span<const std::uint32_t> move_list { move_buf.data(), moves };

        // Exit immediately if this move isn't even pseudo-legal.
        if (std::find(move_list.begin(), move_list.end(), move) == move_list.end())
            break;

        // Make sure the move is fully-legal after playing it and checking legality.
        if (!make_move({ .check_legality=true }, bb_copy, move))
            break;

        legal_moves++;
    }

    return ret.subspan(0, legal_moves);
}