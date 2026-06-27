#include "MyStruct.h"
#include <iostream>

void MyStruct::show()
{
    std::cout << "value " << Value << std::endl;
}

void MyStruct::SetValue(int v)
{
    Value = v;
}