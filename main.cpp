#include <iostream>
#include "board.h"

int main()
{
    Board b;
    clear_board(b);

    std:: cout << "First Square" << b.squares[0] << std::endl;
    return 0 ;
}