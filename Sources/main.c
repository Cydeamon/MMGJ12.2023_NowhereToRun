#include <stdio.h>
#include "CMakeConfig.h"

int main()
{
    printf("Hello, World!\n");
    printf("%s %s\n", PROJECT_LABEL, PROJECT_VER);
    return 0;
}
