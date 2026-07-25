#include <iostream>
#include "board.h"
#include <vector>

int main()
{
    Board b;
    init_starting_position(b);
    std::cout << "Perft(1): " << perft(b, 1) << std::endl; // 20
    std::cout << "Perft(2): " << perft(b, 2) << std::endl; // 400
    std::cout << "Perft(3): " << perft(b, 3) << std::endl; // 8902
    std::cout << "Perft(4): " << perft(b, 4) << std::endl;

    std::cout << "Perft(5): " << perft(b, 5) << std::endl;

    // std::cout << "Perft(6): " << perft(b, 6) << std::endl;
    // perft(6) took me more than 60 seconds! ১১ কোটি পসিবল লিগাল 
}

/*
arafat01rahman@fedora:~/Project/chess_engine$ make clean && make engine && ./engine
rm -f engine
g++ -g -std=c++17 -Wall main.cpp board.cpp -o engine
Perft(1): 20
Perft(2): 400
Perft(3): 8902
Perft(4): 197281
Perft(5): 4865609
Perft(6): 119060324
arafat01rahman@fedora:~/Project/chess_engine$ 

*/