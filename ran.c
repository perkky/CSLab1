#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));

    for (int i = 0; i < 10; i++)
    {
        printf("%.2f\n", ((float)rand()/(float)RAND_MAX)*2 -1);
    }

    return 0;
}
