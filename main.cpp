#include <iostream>
#include "board.h"
#include <vector>
#include "evaluate.h"

int main()
{
    // Board b;
    // init_starting_position(b);
    // std::cout << "Perft(1): " << perft(b, 1) << std::endl; // 20
    // std::cout << "Perft(2): " << perft(b, 2) << std::endl; // 400
    // std::cout << "Perft(3): " << perft(b, 3) << std::endl; // 8902
    // std::cout << "Perft(4): " << perft(b, 4) << std::endl;

    // std::cout << "Perft(5): " << perft(b, 5) << std::endl;

    // std::cout << "Perft(6): " << perft(b, 6) << std::endl;
    // perft(6) took me more than 60 seconds! ১১ কোটি পসিবল লিগাল


    Board bc;
    init_starting_position(bc);
    int score = evaluate(bc);
    std::cout << "Starting position score: " << score << std::endl; // Should be 0
}
