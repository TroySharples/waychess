#include "game_state.hpp"

#include "generate_moves.hpp"
#include "make_move.hpp"
#include "zobrist_hash.hpp"
#include "mailbox.hpp"

void game_state::load(const bitboard& bb)
{
    this->bb = bb;
    hash = zobrist::hash_init(mailbox(bb));

    position_history[0] = hash;
}

void game_state::reset()
{
    bb = {};
    hash = {};

    position_history.fill(0);
    pv.reset();
    km.reset();

    root_ply = {};
}

std::span<const std::uint32_t> game_state::get_pv(std::size_t ply) noexcept
{
    // Initialise the return value with the stored PV, possibly containing illegal moves.
    std::span<const std::uint32_t> ret = pv.table[ply];

    std::size_t legal_moves {};
    std::vector<std::uint32_t> unmake_buf;
    while (legal_moves < ret.size())
    {
        // Return early if we're in a repetition-draw.
        if (is_repetition_draw())
            break;

        const std::uint32_t move { ret[legal_moves] };

        // Is this move in the list of pseudo-legal moves?
        std::array<std::uint32_t, MAX_MOVES_PER_POSITION> move_buf;
        const std::size_t moves { generate_pseudo_legal_moves(bb, move_buf) };
        const std::span<const std::uint32_t> move_list { move_buf.data(), moves };

        // Exit immediately if this move isn't even pseudo-legal.
        if (std::find(move_list.begin(), move_list.end(), move) == move_list.end())
            break;

        // Make sure the move is fully-legal after playing it and checking legality.
        std::uint32_t unmake;
        const bool legal { make_move({ .check_legality=true }, *this, move, unmake) };
        unmake_buf.push_back(unmake);
        if (!legal)
            break;

        legal_moves++;
    }

    // Restore our game-state to how it was before the call.
    std::for_each(unmake_buf.rbegin(), unmake_buf.rend(), [this] (const std::uint32_t& unmake) { unmake_move(*this, unmake); });

    return ret.subspan(0, legal_moves);
}