#include "math.h"
#include <iostream>

int add(int a, int b)
{
    return a + b;
}

int increment(int &a)
{
    return ++a;
}



void printPoint(const Point &p)
{
    std::cout << "point is " << p.x << "," << p.y << std::endl;
}