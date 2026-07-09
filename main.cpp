#include <iostream>
#include "board.h"

bool is_on_board(int sq){return (sq & 0x88) == 0;}

int main() {
    //  Print all valid 0x88 squares
    std::cout << "Valid 0x88 squares:\n";
    for (int sq = 0; sq < 128; sq++) {
        if ((sq & 0x88) == 0) {  // Valid square kina?
            int rank = sq >> 4;
            int file = sq & 7;
            std::cout << "Square " << sq 
                      << " = rank " << rank 
                      << ", file " << file << "\n";
        }
    }
    
    // Test the board
    Board b,c;
    clear_board(b);
    std::cout << "\nBoard[0] = " << b.squares[0] << " (should be 0)\n";
    // Determine the square index for e4 (rank=4, file=4)
    int e4rank = 3, e4file = 4;
    int Teste4 = e4rank << 4 | e4file ;
    std::cout <<"Square Number for e4 = " <<Teste4 << std::endl;
    init_starting_position(c);
    print_board(c);
}