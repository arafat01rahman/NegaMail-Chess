#include "board.h"
#include <iostream>
#include <vector>
#include <cstdint>

int color_of(int piece)
{
    if (piece > 0)
        return 1;
    if (piece < 0)
        return -1;
    return 0;
}

int type_of(int piece)
{
    if (piece == EMPTY)
        return 0;
    if (piece < 0)
        return -piece;
    return piece;
}

void clear_board(Board &b)
{
    for (int i = 0; i < 128; i++)
    {
        b.squares[i] = 0;
    }
    b.en_passant_square = -1;
    b.castling_rights = 0;
    b.side_to_move = 1;
}

bool is_on_board(int sq)
{
    return (sq & 0x88) == 0;
}

void init_starting_position(Board &b)
{
    clear_board(b);
    b.en_passant_square = -1;
    b.castling_rights = 1 | 2 | 4 | 8; // All rights initially
    b.side_to_move = 1;

    int starting[8][8] = {
        {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK},
        {W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN},
        {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK}};
    // Copy the 8x8 array to the 128-slot 0x88 board
    // starting array is defined , but we need to also find out the index of each square for further calculations
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int sq = rank * 16 + file;
            b.squares[sq] = starting[rank][file];
        }
    }
}

char piece_to_char(int piece)
{
    switch (piece)
    {
    case W_PAWN:
        return 'P';
    case W_KNIGHT:
        return 'N';
    case W_BISHOP:
        return 'B';
    case W_ROOK:
        return 'R';
    case W_QUEEN:
        return 'Q';
    case W_KING:
        return 'K';
    case B_PAWN:
        return 'p';
    case B_KNIGHT:
        return 'n';
    case B_BISHOP:
        return 'b';
    case B_ROOK:
        return 'r';
    case B_QUEEN:
        return 'q';
    case B_KING:
        return 'k';
    default:
        return '.';
    }
}

void print_board(Board &b)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            int sq = rank * 16 + file;
            std::cout << piece_to_char(b.squares[sq]) << " ";
        }
        std::cout << '\n';
    }
}

void print_move(const Move &m)
{
    std::cout << "from " << m.from << " to " << m.to << std::endl;
}

bool is_on_starting_rank(int from, int color)
{
    if (color == 1 && from >= 16 && from <= 23)
        return true;
    if (color == -1 && from >= 96 && from <= 103)
        return true;
    return false;
}

void generate_pawn_moves(const Board &b, int color, std::vector<Move> &moves)
{
    int direction = (color == 1) ? 16 : -16;

    for (int from = 0; from < 128; from++)
    {
        if (b.squares[from] != color * W_PAWN)
            continue;
        // Single push
        //  -> can be normal single push or a promotion
        int to = from + direction;
        if (b.squares[to] == EMPTY && is_on_board(to))
        {
            if ((color == 1 && to >= 112 && to <= 119) || (color == -1 && to >= 0 && to <= 7)) // is promotable
            {
                if (color == 1)
                {
                    moves.push_back({from, to, W_KNIGHT, MOVE_PROMOTION});
                    moves.push_back({from, to, W_BISHOP, MOVE_PROMOTION});
                    moves.push_back({from, to, W_ROOK, MOVE_PROMOTION});
                    moves.push_back({from, to, W_QUEEN, MOVE_PROMOTION});
                }
                else
                {
                    moves.push_back({from, to, B_KNIGHT, MOVE_PROMOTION});
                    moves.push_back({from, to, B_BISHOP, MOVE_PROMOTION});
                    moves.push_back({from, to, B_ROOK, MOVE_PROMOTION});
                    moves.push_back({from, to, B_QUEEN, MOVE_PROMOTION});
                }
            }
            else
            {
                moves.push_back({from, to, 0, MOVE_NORMAL});
            }

            // double push , because single push succeded
            if (is_on_starting_rank(from, color))
            {
                int oneStep = from + direction;
                int twoStep = from + 2 * direction;
                if (is_on_board(twoStep) && b.squares[oneStep] == EMPTY && b.squares[twoStep] == EMPTY)
                {
                    moves.push_back({from, twoStep, 0, MOVE_DOUBLE_PAWN_PUSH});
                }
            }
        }

        // Now capturing ,
        // 1. Capture with a promotion 2. Nomral capturing
        int capture_offset[2] = {direction + 1, direction - 1};
        for (int i = 0; i < 2; i++)
        {
            int to = from + capture_offset[i];

            // Check: on board, has enemy piece
            if (is_on_board(to) && b.squares[to] != EMPTY && color_of(b.squares[to]) != color)
            {
                // Check if promotion
                if ((color == 1 && to >= 112 && to <= 119) || (color == -1 && to >= 0 && to <= 7))
                {
                    // Add promotion moves
                    if (color == 1)
                    {
                        moves.push_back({from, to, W_KNIGHT, MOVE_PROMOTION});
                        moves.push_back({from, to, W_BISHOP, MOVE_PROMOTION});
                        moves.push_back({from, to, W_ROOK, MOVE_PROMOTION});
                        moves.push_back({from, to, W_QUEEN, MOVE_PROMOTION});
                    }
                    else
                    {
                        moves.push_back({from, to, B_KNIGHT, MOVE_PROMOTION});
                        moves.push_back({from, to, B_BISHOP, MOVE_PROMOTION});
                        moves.push_back({from, to, B_ROOK, MOVE_PROMOTION});
                        moves.push_back({from, to, B_QUEEN, MOVE_PROMOTION});
                    }
                }
                else
                {
                    moves.push_back({from, to, 0, MOVE_NORMAL});
                }
                // En passant capture  (this block was misplaced; we move it outside)
            }
        }

        // -------- ADDED: En passant captures --------
        if (b.en_passant_square != -1)
        {
            int to = b.en_passant_square;
            // Pawn moves diagonally: difference ±15 or ±17
            int diff = to - from;
            if ((diff == 15 || diff == -15 || diff == 17 || diff == -17) &&
                is_on_board(to))
            {
                // Ensure pawn is on correct rank for en passant
                // White pawns: rank 5 (squares 64..71), Black pawns: rank 4 (squares 48..55)
                if ((color == 1 && (from >= 64 && from <= 71)) ||
                    (color == -1 && (from >= 48 && from <= 55)))
                {
                    moves.push_back({from, to, 0, MOVE_ENPASSANT});
                }
            }
        }
        // --------------------------------------------
    }
}

void generate_knight_moves(const Board &b, int from, int color, std::vector<Move> &moves)
{
    for (int i = 0; i < 8; i++)
    {
        int to = from + knight_offset[i];
        if (is_on_board(to) && (b.squares[to] == EMPTY || color_of(b.squares[to]) != color))
        {
            moves.push_back({from, to, 0, MOVE_NORMAL});
        }
    }
}

void generate_king_moves(const Board &b, int from, int color, std::vector<Move> &moves)
{
    for (int i = 0; i < 8; i++)
    {
        int to = from + king_offset[i];
        if (is_on_board(to) && ((color_of(b.squares[to]) != color) || b.squares[to] == EMPTY))
        {
            moves.push_back({from, to, 0, MOVE_NORMAL});
        }
    }
}

void generate_slider_moves(const Board &b, int from, int color, const int *offsets, int dir_offsets, std::vector<Move> &moves)
{
    for (int i = 0; i < dir_offsets; i++)
    {
        int to = from + offsets[i];
        while (is_on_board(to))
        {
            if (b.squares[to] == EMPTY)
            {
                moves.push_back({from, to, 0, MOVE_NORMAL});
            }
            else
            {
                if (color_of(b.squares[to]) != color)
                {
                    moves.push_back({from, to, 0, MOVE_NORMAL});
                }
                break;
            }
            to += offsets[i];
        }
    }
}

void generate_moves(const Board &b, int color, std::vector<Move> &moves)
{
    moves.clear();

    // Generate pawn moves once (they handle all pawns internally)
    generate_pawn_moves(b, color, moves);

    // Generate moves for all other pieces
    for (int from = 0; from < 128; from++)
    {
        int piece = b.squares[from];
        if (piece == EMPTY || color_of(piece) != color)
            continue;

        int piece_type = type_of(piece);

        switch (piece_type)
        {
        case W_KNIGHT:
            generate_knight_moves(b, from, color, moves);
            break;
        case W_BISHOP:
            generate_slider_moves(b, from, color, bishop_offset, 4, moves);
            break;
        case W_ROOK:
            generate_slider_moves(b, from, color, rook_offset, 4, moves);
            break;
        case W_QUEEN:
            generate_slider_moves(b, from, color, queen_offset, 8, moves);
            break;
        case W_KING:
            generate_king_moves(b, from, color, moves);

            // White castling
            if (color == 1 && from == 4)
            {
                if (b.castling_rights & 1)
                {
                    if (b.squares[5] == EMPTY && b.squares[6] == EMPTY && b.squares[7] == W_ROOK &&
                        !is_square_attacked(b, 4, -1) && !is_square_attacked(b, 5, -1) && !is_square_attacked(b, 6, -1))
                    {
                        moves.push_back({4, 6, 0, MOVE_CASTLING});
                    }
                }
                if (b.castling_rights & 2)
                {
                    if (b.squares[3] == EMPTY && b.squares[2] == EMPTY && b.squares[1] == EMPTY && b.squares[0] == W_ROOK &&
                        !is_square_attacked(b, 4, -1) && !is_square_attacked(b, 3, -1) && !is_square_attacked(b, 2, -1))
                    {
                        moves.push_back({4, 2, 0, MOVE_CASTLING});
                    }
                }
            }

            // Black castling
            if (color == -1 && from == 116)
            {
                if (b.castling_rights & 4)
                {
                    if (b.squares[117] == EMPTY && b.squares[118] == EMPTY && b.squares[119] == B_ROOK &&
                        !is_square_attacked(b, 116, 1) && !is_square_attacked(b, 117, 1) && !is_square_attacked(b, 118, 1))
                    {
                        moves.push_back({116, 118, 0, MOVE_CASTLING});
                    }
                }
                if (b.castling_rights & 8)
                {
                    // Queenside: destination is c8 (114), NOT a8 (112)
                    if (b.squares[115] == EMPTY && b.squares[114] == EMPTY && b.squares[113] == EMPTY && b.squares[112] == B_ROOK &&
                        !is_square_attacked(b, 116, 1) && !is_square_attacked(b, 115, 1) && !is_square_attacked(b, 114, 1))
                    {
                        moves.push_back({116, 114, 0, MOVE_CASTLING});
                    }
                }
            }
            break;
        }
    }
}

UndoInfo save_state(const Board &b)
{
    UndoInfo u;
    u.captured_piece = 0;
    u.captured_square = 0;
    u.en_passant_square = b.en_passant_square;
    u.castling_rights = b.castling_rights;
    u.side_to_move = b.side_to_move;

    return u;
}

UndoInfo make_move(Board &b, const Move &m)
{
    // 1. Save state
    UndoInfo u = save_state(b);

    // 2. Store captured piece (BEFORE moving)
    u.captured_piece = b.squares[m.to];
    u.captured_square = m.to;

    // 3. Get piece and color BEFORE moving
    int piece = b.squares[m.from];
    int color = color_of(piece);

    // 4. Move the piece
    b.squares[m.to] = piece;
    b.squares[m.from] = EMPTY;

    // 4.5. Revoke castling rights if king or rook moved, or rook was captured
    if (type_of(piece) == W_KING)
    {
        if (color == 1)
            b.castling_rights &= ~(1 | 2);
        else
            b.castling_rights &= ~(4 | 8);
    }
    if (m.from == 0)
        b.castling_rights &= ~2;
    if (m.from == 7)
        b.castling_rights &= ~1;
    if (m.from == 112)
        b.castling_rights &= ~8;
    if (m.from == 119)
        b.castling_rights &= ~4;

    if (u.captured_square == 0)
        b.castling_rights &= ~2;
    if (u.captured_square == 7)
        b.castling_rights &= ~1;
    if (u.captured_square == 112)
        b.castling_rights &= ~8;
    if (u.captured_square == 119)
        b.castling_rights &= ~4;

    // 5. Handle special moves
    if (m.flag == MOVE_PROMOTION)
    {
        // Replace pawn with promoted piece
        b.squares[m.to] = m.promotion;
    }
    else if (m.flag == MOVE_ENPASSANT)
    {
        // Remove the captured pawn from the en passant square
        int captured_sq = m.to + ((color == 1) ? -16 : 16);
        u.captured_piece = b.squares[captured_sq];
        u.captured_square = captured_sq;
        b.squares[captured_sq] = EMPTY;
    }
    else if (m.flag == MOVE_CASTLING)
    {
        // Move the rook
        if (m.to == 6) // White kingside: king e1→g1, rook h1→f1
        {
            b.squares[5] = W_ROOK; // Rook to f1
            b.squares[7] = EMPTY;  // Clear h1
        }
        else if (m.to == 2) // White queenside: king e1→c1, rook a1→d1
        {
            b.squares[3] = W_ROOK; // Rook to d1
            b.squares[0] = EMPTY;  // Clear a1
        }
        else if (m.to == 118) // Black kingside: king e8→g8, rook h8→f8
        {
            b.squares[117] = B_ROOK; // Rook to f8
            b.squares[119] = EMPTY;  // Clear h8
        }
        else if (m.to == 114) // Black queenside: king e8→c8, rook a8→d8   (changed from 112 to 114)
        {
            b.squares[115] = B_ROOK; // Rook to d8
            b.squares[112] = EMPTY;  // Clear a8
        }
    }

    // 6. Update en passant square
    b.en_passant_square = -1;
    if (m.flag == MOVE_DOUBLE_PAWN_PUSH)
    {
        int direction = (color == 1) ? 16 : -16;
        b.en_passant_square = m.from + direction;
    }

    // 7. Toggle side to move
    b.side_to_move = -b.side_to_move;
    return u;
}   
void unmake_move(Board &b, const Move &m, const UndoInfo &u)
{
    // Handle en passant
    if (m.flag == MOVE_ENPASSANT)
    {
        b.squares[u.captured_square] = u.captured_piece;
        b.squares[m.from] = b.squares[m.to];
        b.squares[m.to] = EMPTY;

        b.en_passant_square = u.en_passant_square;
        b.castling_rights = u.castling_rights;
        b.side_to_move = u.side_to_move;
        return;
    }

    // Handle castling
    if (m.flag == MOVE_CASTLING)
    {
        // Move the king back
        b.squares[m.from] = b.squares[m.to];
        b.squares[m.to] = u.captured_piece;

        // Move the rook back
        if (m.to == 6) // White kingside
        {
            b.squares[7] = W_ROOK; // Restore rook to h1
            b.squares[5] = EMPTY;  // Clear f1
        }
        else if (m.to == 2) // White queenside
        {
            b.squares[0] = W_ROOK; // Restore rook to a1
            b.squares[3] = EMPTY;  // Clear d1
        }
        else if (m.to == 118) // Black kingside
        {
            b.squares[119] = B_ROOK; // Restore rook to h8
            b.squares[117] = EMPTY;  // Clear f8
        }
        else if (m.to == 114) // Black queenside
        {
            b.squares[112] = B_ROOK; // Restore rook to a8
            b.squares[115] = EMPTY;  // Clear d8
        }

        // Restore state
        b.en_passant_square = u.en_passant_square;
        b.castling_rights = u.castling_rights;
        b.side_to_move = u.side_to_move;
        return;
    }

    // Normal unmake (for non-special moves)
    b.squares[m.from] = b.squares[m.to];
    b.squares[m.to] = u.captured_piece;

    // Handle promotion
    if (m.flag == MOVE_PROMOTION)
    {
        int color = (b.squares[m.from] > 0) ? 1 : -1;
        b.squares[m.from] = color * W_PAWN;
    }

    // Restore state
    b.en_passant_square = u.en_passant_square;
    b.castling_rights = u.castling_rights;
    b.side_to_move = u.side_to_move;
}

bool is_square_attacked(const Board &b, int square, int attacker_color)
{
    // check if any knights are on the way to
    for (int i = 0; i < 8; i++)
    {
        int from = square + knight_offset[i];
        if (is_on_board(from))
        {
            int piece = b.squares[from];
            if (piece != EMPTY && type_of(piece) == W_KNIGHT && color_of(piece) == attacker_color)
            {
                return true;
            }
        }
    }
    // Pawn attacks checking on a squrare
    if (attacker_color == 1) // W_pawns attacks upward
    {
        int from1 = square - 15;
        int from2 = square - 17;
        if (is_on_board(from1) && b.squares[from1] == W_PAWN)
            return true;
        if (is_on_board(from2) && b.squares[from2] == W_PAWN)
            return true;
    }
    if (attacker_color == -1) // B_pawns attack downwards
    {
        int from1 = square + 15;
        int from2 = square + 17;
        if (is_on_board(from1) && b.squares[from1] == B_PAWN)
            return true;
        if (is_on_board(from2) && b.squares[from2] == B_PAWN)
            return true;
    }

    // attack by king check

    if (attacker_color == 1) // W_king will be attacking
    {
        for (int i = 0; i < 8; i++)
        {
            int from = square + king_offset[i];
            if (is_on_board(from) && b.squares[from] == W_KING)
            {
                return true;
            }
        }
    }
    if (attacker_color == -1) // B_king will be attacking
    {
        for (int i = 0; i < 8; i++)
        {
            int from = square + king_offset[i];
            if (is_on_board(from) && b.squares[from] == B_KING)
            {
                return true;
            }
        }
    }

    // Slider attacks (attacks by queen , rooks and bishop)
    //
    // Bishop + queen
    for (int i = 0; i < 4; i++)
    {
        int from = square + bishop_offset[i];
        while (is_on_board(from))
        {
            int piece = b.squares[from];
            if (piece != EMPTY)
            {
                if (color_of(piece) == attacker_color &&
                    (type_of(piece) == W_BISHOP || type_of(piece) == W_QUEEN))
                {
                    return true;
                }
                break; // Any piece blocks further
            }
            from += bishop_offset[i];
        }
    }

    // Rook + queen
    for (int i = 0; i < 4; i++)
    {
        int from = square + rook_offset[i];
        while (is_on_board(from))
        {
            int piece = b.squares[from];
            if (piece != EMPTY)
            {
                if (color_of(piece) == attacker_color &&
                    (type_of(piece) == W_ROOK || type_of(piece) == W_QUEEN))
                {
                    return true;
                }
                break;
            }
            from += rook_offset[i];
        }
    }
    return false;
}

int find_king(const Board &b, int color)
{
    for (int i = 0; i < 128; i++)
    {
        if (b.squares[i] == color * W_KING)
        {
            return i;
        }
    }
    return -1;
}

bool is_in_check(const Board &b, int color)
{
    int king = find_king(b, color);
    if (king == -1)
        return false;
    return is_square_attacked(b, king, -color);
}

void generate_legal_moves(Board &b, int color, std::vector<Move> &moves)
{
    moves.clear();
    std::vector<Move> pseudo_moves;
    generate_moves(b, color, pseudo_moves);
    for (const Move &m : pseudo_moves)
    {
        UndoInfo u = make_move(b, m);
        if (!is_in_check(b, color))
        {
            // keep the move
            moves.push_back(m);
        }
        unmake_move(b, m, u);
    }
}

uint64_t perft(Board &b, int depth)
{
    if (depth == 0)
    {
        return 1;
    }
    uint64_t nodes = 0;
    std::vector<Move> moves;
    generate_legal_moves(b, b.side_to_move, moves);
    for (const Move &m : moves)
    {
        UndoInfo undo = make_move(b, m);
        nodes += perft(b, depth - 1);
        unmake_move(b, m, undo);
    }
    return nodes;
}

