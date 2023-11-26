#include <iostream>
#include "CMakeConfig.h"

int main()
{
    std::cout << "Hello, World!\n";
    std::cout << PROJECT_LABEL << " " << PROJECT_VER << "\n";
    return 0;
}
