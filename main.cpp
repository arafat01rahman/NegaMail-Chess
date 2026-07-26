#include <iostream>
#include "board.h"
#include <vector>
#include "evaluate.h"
#include "search.h"

int main()
{
    Board b;
    init_starting_position(b);

    Move best = find_best_move(b, 3);
    std::cout << "Best move: from " << best.from << " to " << best.to << std::endl;
}
