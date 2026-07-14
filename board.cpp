#include "board.h"
#include <iostream>

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

void init_starting_position(Board &b)
{
    clear_board(b);

    int starting[8][8] = {
        {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK},
        {B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN},
        {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK}};

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
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            std::cout << piece_to_char(b.squares[i * 16 + j]) << " ";
        }
        std::cout << '\n';
    }
}

void print_move(const move & m)
{
    std::cout << "from " << m.from << " to "<< m.to << std::endl;
}