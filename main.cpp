#include <iostream>
#include "board.h"
#include "math.h"
#include <vector>
#include "MyStruct.h"

int main()
{

    Board b;
    clear_board(b);

    std::cout << "First Square" << b.squares[0] << std::endl;

    std::cout << "Hello" << std::endl;
    std::cout << "adding 3+3 = " << add(3, 3) << std::endl;
    int x = 1;
    std::cout << "incremented 1 = " << increment(x) << std::endl;
    Point p1 = {1, 2};
    printPoint(p1);

    std::vector<int> vec = {1, 2, 3, 4};
    auto it = vec.begin() + 1;
    std::cout << "Vector at index 0 = " << *it << std::endl;

    MyStruct ms1 ;
    ms1.Value = 33;
    std:: cout << ms1.Value << std::endl;
    ms1.show();
}
