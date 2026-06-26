#include <iostream>
#include "board.h"
#include "math.h"
#include <vector>

using namespace std;

int main()
{

    Board b;
    clear_board(b);

    std::cout << "First Square" << b.squares[0] << std::endl;

    std::cout << "Hello" << std::endl;
    std::cout << "adding 3+3 = " << add(3, 3) << std::endl;
    int x = 1;
    cout << "incremented 1 = " << increment(x) << endl;
    Point p1 = {1, 2};
    printPoint(p1);

    vector<int> vec = {1, 2, 3, 4};
    auto it = vec.begin() + 1;
    cout << "Vector at index 0 = " << *it << endl;
}
