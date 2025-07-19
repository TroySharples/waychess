#include "mailbox.hpp"

#include "bitboard.hpp"
#include "utility/coordinates.hpp"

#include <sstream>

namespace
{

constexpr const char* ANSI_RESET = "\033[0m";

constexpr const char* ANSI_BACKGROUND_CYAN  = "\033[46m";
constexpr const char* ANSI_BACKGROUND_WHITE = "\033[47m";

}

mailbox::mailbox(const bitboard& bb)
{
    for (std::size_t i = 0; i < squares.size(); i++)
    {
        const std::uint64_t b { 1ULL << i };

        if (b & bb.boards[piece_idx::w_pawn])
            squares[i] = piece_idx::w_pawn;
        else if (b & bb.boards[piece_idx::w_knight])
            squares[i] = piece_idx::w_knight;
        else if (b & bb.boards[piece_idx::w_king])
            squares[i] = piece_idx::w_king;
        else if (b & bb.boards[piece_idx::w_queen])
            squares[i] = piece_idx::w_queen;
        else if (b & bb.boards[piece_idx::w_rook])
            squares[i] = piece_idx::w_rook;
        else if (b & bb.boards[piece_idx::w_bishop])
            squares[i] = piece_idx::w_bishop;
        else if (b & bb.boards[piece_idx::b_pawn])
            squares[i] = piece_idx::b_pawn;
        else if (b & bb.boards[piece_idx::b_knight])
            squares[i] = piece_idx::b_knight;
        else if (b & bb.boards[piece_idx::b_king])
            squares[i] = piece_idx::b_king;
        else if (b & bb.boards[piece_idx::b_queen])
            squares[i] = piece_idx::b_queen;
        else if (b & bb.boards[piece_idx::b_rook])
            squares[i] = piece_idx::b_rook;
        else if (b & bb.boards[piece_idx::b_bishop])
            squares[i] = piece_idx::b_bishop;
        else
            squares[i] = piece_idx::empty;
    }

    if (bb.en_passent_mb)
        en_passent_square.emplace(std::countr_zero(bb.en_passent_mb));

    castling_w_ks = bb.castling & bitboard::CASTLING_W_KS;
    castling_w_qs = bb.castling & bitboard::CASTLING_W_QS;
    castling_b_ks = bb.castling & bitboard::CASTLING_B_KS;
    castling_b_qs = bb.castling & bitboard::CASTLING_B_QS;

    ply_counter = bb.ply_counter;
    ply_50m     = bb.ply_50m;
}

mailbox::mailbox(std::string_view fen)
{
    // We ensure safe access using the at method.
    std::uint8_t pos { 0 };

    // Start with the piece placement with everything filled empty.
    squares.fill(piece_idx::empty);
    for (std::uint8_t i = 0; i < 8; i++)
    {
        for (std::uint8_t j = 0; j < 8; )
        {
            const char c = fen.at(pos++);
            
            // If it's a number between 1 and 1 + j (more horrible ASCII arithmetic)
            // we insert that many empty squares.
            if ('1' <= c and c <= '1' + static_cast<char>(8-j))
                j += (c - '0');
            else
                squares[8*(8-i-1)+j++] = from_fen_char(c);
        }
        
        // Each rank should be separated by a forward-slash.
        if (i < 7 && fen.at(pos++) != '/')
            throw std::invalid_argument("Expected forward-slash charactor in fen string");
    }
    if (fen.at(pos++) != ' ')
        throw std::invalid_argument("Expected space charactor in fen string");

    // Side to move. We use the LSB of the ply counter to represent which side is to move.
    switch (fen.at(pos++))
    {
        case 'w': ply_counter = 0; break;
        case 'b': ply_counter = 1; break;
        default: throw std::invalid_argument("Unknown side to move in fen string");
    }
    if (fen.at(pos++) != ' ')
        throw std::invalid_argument("Expected space charactor in fen string");

    // Castling ability. We can actually sucessfully parse invalid fen strings here, but
    // that's the callers problem (TM).
    castling_w_ks = false;
    castling_w_qs = false;
    castling_b_ks = false;
    castling_b_qs = false;
    for (char c = fen.at(pos++); c != ' '; c = fen.at(pos++))
    {
        switch (c)
        {
            case 'K': castling_w_ks = true; break;
            case 'Q': castling_w_qs = true; break;
            case 'k': castling_b_ks = true; break;
            case 'q': castling_b_qs = true; break;
            case '-': break;
            default:
                throw std::invalid_argument("Unexpected castling charactor in fen string");
        }
    }

    // En-passent target square - this one's easy.
    if (const char c = fen.at(pos); c == '-')
    {
        pos++;
    }
    else
    {
        en_passent_square.emplace(from_coordinates_str(fen.substr(pos, 2)));
        pos+=2;
    }
    if (fen.at(pos++) != ' ')
        throw std::invalid_argument("Expected space charactor in fen string");

    // The last two values in the fen string are integers of undetermined length. We use
    // the iostream parsing functions for this.
    std::stringstream ss;
    ss << fen.substr(pos);
    
    // Half-move clock.
    ss >> ply_50m;

    // Full move counter. This was initialised in the side-to-move section, so we can only
    // add to this value.
    {
        std::uint16_t move_counter;
        ss >> move_counter;
        ply_counter += 2*(move_counter-1);
    }
}

std::string mailbox::get_fen_string() const noexcept
{
    std::stringstream ss;

    // The endianness is back-to-front with FEN - we have to start from A8 and work down to H1.
    for (std::uint8_t i = 0; i < 8; i++)
    {
        std::size_t empty_counter { 0 };
        for (std::uint8_t j = 0; j < 8; j++)
        {
            const auto p = squares[8*(8-i-1)+j];
            if (p == piece_idx::empty)
            {
                empty_counter++;
            }
            else
            {
                if (empty_counter > 0)
                    ss << empty_counter;
                empty_counter = 0;
                ss << to_fen_char(p);
            }
        }
        if (empty_counter > 0)
            ss << empty_counter;
        empty_counter = 0;
        if (i < 7)
            ss << '/';
    }
    ss << ' ';

    // Side to move.
    ss << (ply_counter & 1ULL ? 'b' : 'w') << ' ';

    // Castling ability.
    if (castling_w_ks || castling_w_qs || castling_b_ks || castling_b_qs)
    {
        if (castling_w_ks) ss << 'K';
        if (castling_w_qs) ss << 'Q';
        if (castling_b_ks) ss << 'k';
        if (castling_b_qs) ss << 'q';
    }
    else
    {
        ss << '-';
    }
    ss << ' ';

    // En-passent target square.
    if (en_passent_square.has_value())
        ss << to_coordinates_str(*en_passent_square);
    else
        ss << '-';
    ss << ' ';

    // Half-move clock.
    ss << ply_50m << ' ';

    // Full move counter.
    ss << 1+ply_counter/2;

    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const mailbox& v)
{
    os << ANSI_RESET;
    for (std::uint8_t rank = 0; rank < 8; rank++)
    {
        for (std::uint8_t file = 0; file < 8; file++)
            os << (WHITE_SQUARES & get_bitboard_mailbox_piece(rank*8+file) ? ANSI_BACKGROUND_CYAN : ANSI_BACKGROUND_WHITE) << v.squares[(7-rank)*8+file];
        os << ANSI_RESET << '\n';
    }
    return os;
}
