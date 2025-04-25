

void get_white_pawn_moves(position current_position, std::vector<position>& next_positions)
{
    position next_position = current_position;
    next_position.white_to_move     = false;
    next_position.en_passent_square = 0;

    for (bitboard white_pawns { current_position.white_pieces & current_position.pawns }; white_pawns; )
    {
        const bitboard pawn { ls1b_isolate(white_pawns) };

        // Calcualte attacking moves.
        {
            const bitboard attacked_squares { get_white_pawn_all_attacked_squares_from_bitboard(pawn) };

            // Regular attacks.
            for (bitboard moves { attacked_squares & current_position.black_pieces }; moves; )
            {
                const bitboard move { ls1b_isolate(moves) };
                
                {
                    next_position.pawns        = (current_position.pawns        ^ pawn) | move;
                    next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
                    next_position.black_pieces = current_position.black_pieces  ^ move;

                    next_positions.push_back(next_position);
                }

                moves ^= move;
            }

            // En-passent attack. We don't have to loop here as there is at most one en-passent capture square in the position.
            if (const bitboard move { attacked_squares & current_position.en_passent_square }; move)
            {
                next_position.pawns        = (current_position.pawns        ^ pawn) | move;
                next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
                next_position.black_pieces = current_position.black_pieces  ^ current_position.en_passent_piece;

                next_positions.push_back(next_position);
            }
        }

        // Calculate single-push moves. There can be at most one of these.
        if (const bitboard move { get_white_pawn_single_push_squares_from_bitboard(pawn, ~(current_position.white_pieces | current_position.black_pieces)) }; move)
        {
            next_position.pawns        = (current_position.pawns        ^ pawn) | move;
            next_position.white_pieces = (current_position.white_pieces ^ pawn) | move;
            
            next_positions.push_back(next_position);
        }

        // Calculate double-push moves and remember to set the en-passent data (and clear it after). It is important to do this first to clear the en-passent bit.
        if (const bitboard move { get_white_pawn_double_push_squares_from_bitboard(pawn, ~(current_position.white_pieces | current_position.black_pieces)) }; move)
        {
            next_position.pawns             = (current_position.pawns        ^ pawn) | move;
            next_position.white_pieces      = (current_position.white_pieces ^ pawn) | move;
            next_position.en_passent_piece  = move;
            next_position.en_passent_square = pawn << 8;
            
            next_positions.push_back(next_position);
            next_position.en_passent_square = 0;
        }

        // TODO: Handle promotion.

        // Mask off this bit before looping again.
        white_pawns ^= pawn;
    }
}

void get_black_pawn_moves(position current_position, std::vector<position>& next_positions)
{

}

void get_knight_moves(position current_position, std::vector<position>& next_positions)
{
    position next_position = current_position;
    next_position.white_to_move = !current_position.white_to_move;
    next_position.en_passent_square = 0;
    
    for (bitboard to_move_knights { to_move_pieces(current_position) & current_position.knights }; to_move_knights; )
    {
        const bitboard knight_bitboard { ls1b_isolate(to_move_knights) };
        const mailbox knight_mailbox { bitscan_forward( to_move_knights) };

        for (bitboard moves { get_knight_attacked_squares_from_mailbox(knight_mailbox) & ~to_move_pieces(current_position) }; moves; )
        {
            const bitboard move { ls1b_isolate(moves) };
            
            {
                next_position.knights = (current_position.knights ^ knight_bitboard) | move;
                opponent_pieces(next_position) = (to_move_pieces(current_position) ^ knight_bitboard) | move;
                to_move_pieces(next_position)  = opponent_pieces(current_position) & ~move;

                next_positions.push_back(next_position);
            }

            moves ^= move;
        }

        to_move_knights ^= knight_bitboard;
    }
}

void get_rook_moves(position current_position, std::vector<position>& next_positions)
{
    position next_position = current_position;
    next_position.white_to_move = !current_position.white_to_move;
    next_position.en_passent_square = 0;
    
    for (bitboard to_move_rooks { to_move_pieces(current_position) & current_position.rooks }; to_move_rooks; )
    {
        const bitboard rook_bitboard { ls1b_isolate(to_move_rooks) };
        const mailbox rook_mailbox { bitscan_forward( to_move_rooks) };

        for (bitboard moves { get_rook_attacked_squares_from_mailbox(rook_mailbox, current_position.white_pieces | current_position.black_pieces) & ~to_move_pieces(current_position) }; moves; )
        {
            const bitboard move { ls1b_isolate(moves) };
            
            {
                next_position.rooks = (current_position.rooks ^ rook_bitboard) | move;
                opponent_pieces(next_position) = (to_move_pieces(current_position) ^ rook_bitboard) | move;
                to_move_pieces(next_position)  = opponent_pieces(current_position) & ~move;

                next_positions.push_back(next_position);
            }

            moves ^= move;
        }

        to_move_rooks ^= rook_bitboard;
    }
}

std::vector<position> get_next_positions(position pos)
{
    std::vector<position> ret;

    pos.white_to_move ? get_white_pawn_moves(pos, ret) : get_black_pawn_moves(pos, ret);

    get_knight_moves(pos, ret);
    get_rook_moves(pos, ret);

    return ret;
}