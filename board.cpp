#include "board.h"

void clear_board(Board& b)
{
    for(int i = 0 ; i < 128 ; i++)
    {
        b.squares[i] = 0 ;
    }
}