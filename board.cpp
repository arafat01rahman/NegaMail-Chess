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

void print_move(const move &m)
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

void generate_pawn_moves(const Board &b, int color, std::vector<move> &moves)
{
    int direction;
    if (color == 1)
    {
        direction = 16;
    }
    else if (color == -1)
    {
        direction = -16;
    }
    else
        return;

    // iterating all 128 squares as a piece , if its of given color
    //  then calculate direction
    //  then , check if its on board and empty
    //  -> if yes, then take note on that possible move.(push back)
    for (int from = 0; from < 128; from++)
    {
        int piece = b.squares[from];
        if (piece == color * W_PAWN)
        {
            // Single push
            int to = from + direction;
            if (is_on_board(to) && b.squares[to] == EMPTY)
            {
                moves.push_back({from, to, 0, MOVE_NORMAL});

                // Double push (ONLY for pawns, and ONLY if single push was possible)
                if (is_on_starting_rank(from, color))
                {
                    int oneStep = from + direction;
                    int twoStep = from + 2 * direction;
                    if (is_on_board(twoStep) &&
                        b.squares[oneStep] == EMPTY &&
                        b.squares[twoStep] == EMPTY)
                    {
                        moves.push_back({from, twoStep, 0, MOVE_DOUBLE_PAWN_PUSH});
                    }
                }
            }
        }
    }
}