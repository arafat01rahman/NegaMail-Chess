#include <iostream>
#include "board.h"
#include "math.h"
#include <vector>
#include "MyStruct.h"
#include <vector>

void PrintVec( std::vector<int> nums)
    {
        for(const auto &i : nums)
        {
            std::cout<< i << " " ;
        }
        std:: cout << std::endl ; 
    }

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

    MyStruct ms1;
    ms1.Value = 33;
    std::cout << ms1.Value << std::endl;
    ms1.show();

    // Vector Learning Part
    std::vector<int> numbers;
    numbers.reserve(10);
    for (int i = 0; i < 10; i++)
    {
        numbers.push_back((i + 1) * 10);
    }
    for (const auto &m : numbers)
    {
        std::cout << m << std::endl;
    }

    // Inserting MyStruct into a vector
    std::vector<MyStruct> MyStructVec;
    MyStruct ms2;
    ms2.Value = 69;
    MyStructVec.push_back(ms2);
    MyStruct s3;
    s3.SetValue(30);
    MyStructVec.push_back(s3);

    for( auto &i : MyStructVec)
    {
        i.show() ;
    }

    //Print Vec FUnction
    PrintVec(numbers);
   
    
}
 