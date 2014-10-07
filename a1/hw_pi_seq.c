#include <stdio.h>
#include <time.h>
#include <stdlib.h>

double rn()
{
    return  ((double)rand()/(double)(RAND_MAX-1.0)) ;
}

int main( int argc, char *argv[] ) {
    int sq_side = 1000, radius = sq_side/2;
    int n = 1000, success = 0;
    double pi = 0;
    srand(time(NULL));
    printf("Sequential MonteCarlo for Pi\n");
    for (int cnt = 0; cnt < n; cnt ++)
    {
        double x = rn();
        double y = rn();
        if ((x*x + y*y) <= 1.0)
        {
            success += 1;
        }
    }
    pi = ((double)success)/((double)n) * 4;
    printf("PI = %1.5f\n", pi);
    printf("PI/4 = %1.5f\n", ((double)success/n));
}

