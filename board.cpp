#include "board.h"
#include <iostream>
#include <vector>

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
                // En passant capture
                if (to == b.en_passant_square)
                {
                    moves.push_back({from, to, 0, MOVE_ENPASSANT});
                }
            }
        }
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

    for (int from = 0; from < 128; from++)
    {
        int piece = b.squares[from];
        if (piece == EMPTY || color_of(piece) != color)
            continue;

        int piece_type = type_of(piece);

        switch (piece_type)
        {
        case W_PAWN:
            generate_pawn_moves(b, color, moves);
            break;
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

            if (color == 1 && from == 4)
            {
                if (b.castling_rights & 1)
                {
                    if (b.squares[5] == EMPTY && b.squares[6] == EMPTY && b.squares[7] == W_ROOK)
                    {
                        moves.push_back({4, 6, 0, MOVE_CASTLING});
                    }
                }
                if (b.castling_rights & 2)
                {
                    if (b.squares[3] == EMPTY && b.squares[2] == EMPTY && b.squares[1] == EMPTY && b.squares[0] == W_ROOK)
                    {
                        moves.push_back({4, 2, 0, MOVE_CASTLING});
                    }
                }
            }

            if (color == -1 && from == 116)
            {
                if (b.castling_rights & 4)
                {
                    if (b.squares[117] == EMPTY && b.squares[118] == EMPTY && b.squares[119] == B_ROOK)
                    {
                        moves.push_back({116, 118, 0, MOVE_CASTLING});
                    }
                }
                if (b.castling_rights & 8)
                {
                    if (b.squares[115] == EMPTY && b.squares[114] == EMPTY && b.squares[113] == EMPTY && b.squares[112] == B_ROOK)
                    {
                        moves.push_back({116, 112, 0, MOVE_CASTLING});
                    }
                }
            }
            break;
        }
    }
}