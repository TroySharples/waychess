#include "game_state.hpp"

#include "zobrist_hash.hpp"
#include "mailbox.hpp"

game_state::game_state(const bitboard& bb)
    : bb(bb), hash(zobrist::hash_init(mailbox(bb)))
{

}