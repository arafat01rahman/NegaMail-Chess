#include <iostream>
#include "board.h"
#include "math.h"

int main()
{
    
    Board b;
    clear_board(b);

    std::cout << "First Square" << b.squares[0] << std::endl;

    std::cout << "Hello" << std::endl;
    std::cout << "adding 3+3 = " << add(3, 3) << std::endl;
    return 0;
}