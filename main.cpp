#include <iostream>
#include "board.h"
#include <vector>

int main()
{
    //  Print all valid 0x88 squares
    // std::cout << "Valid 0x88 squares:\n";
    // for (int sq = 0; sq < 128; sq++)
    // {
    //     if ((sq & 0x88) == 0)
    //     { // Valid square kina?
    //         int rank = sq >> 4;
    //         int file = sq & 7;
    //         std::cout << "Square " << sq
    //                   << " = rank " << rank
    //                   << ", file " << file << "\n";
    //     }
    // }

    // // Test the board
    // Board b, c;
    // clear_board(b);
    // std::cout << "\nBoard[0] = " << b.squares[0] << " (should be 0)\n";
    // // Determine the square index for e4 (rank=4, file=4)
    // int e4rank = 3, e4file = 4;
    // int Teste4 = e4rank << 4 | e4file;
    // std::cout << "Square Number for e4 = " << Teste4 << std::endl;
    // init_starting_position(c);
    // // clear_board(c);
    // print_board(c);
    // std::vector<Move> knight_moves;
    // generate_knight_moves(c, 1, 1, knight_moves);
    // std::cout << "Knight moves: " << knight_moves.size() << std::endl;
    // for (const auto &m : knight_moves)
    // {
    //     print_move(m);
    // }

    // Board f;
    // init_starting_position(f);
    // print_board(f);

    // std::vector<Move> moves;
    // generate_moves(f, 1, moves); // White moves

    // std::cout << "White moves: " << moves.size() << std::endl;
    // for (const auto &m : moves)
    // {
    //     print_move(m);
    // }
    Board ng;
    init_starting_position(ng);
    std::vector<Move> moves;
    generate_moves(ng, 1, moves);
    std::cout << "Moves: " << moves.size() << std::endl; // Should be 20

    std::vector<Move> legal;
    generate_legal_moves(ng, 1, legal);
    std::cout << "Legal moves: " << legal.size() << std::endl; // Should be 20
}