#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/*  globals */
int numnodes,myid,mpi_err;
#define mpi_root 0
/* end globals  */

void init_it(int  *argc, char ***argv);
int nsuccess(int n);
double rn ();

int main( int argc, char *argv[] ) {
    int *myray,*send_ray,*back_ray;
    int count;
    int size,mysize,i,k,j,total;
    int n = 100, *localn;

    init_it(&argc,&argv);
    /* each processor will get count elements from the root */
    count=4;
    myray=(int*)malloc(1*sizeof(int));
    /* create the data to be sent on the root  */
    if(myid == mpi_root){
        size=count*numnodes;
        send_ray=(int*)malloc(numnodes*sizeof(int));
        for (int i = 0 ; i < numnodes; i++) {
            send_ray[i] = 100;
        }
        back_ray=(int*)malloc(numnodes*sizeof(int));
    }
    /* send different data to each processor */
    mpi_err = MPI_Scatter(  send_ray, 1,   MPI_INT,
            myray,    1,   MPI_INT,
            mpi_root,
            MPI_COMM_WORLD);

    //printf("myid= %d total= %d localn = %i\n ",myid,total,(myray[0])); 
    /* each processor does a local sum */
    srand(time(NULL)+myid*numnodes);
    total=nsuccess(myray[0]);
    /* send the local sums back to the root */
    mpi_err = MPI_Gather(&total,    1,  MPI_INT, 
            back_ray, 1,  MPI_INT, 
            mpi_root,                  
            MPI_COMM_WORLD);
    /* the root prints the global sum */
    if(myid == mpi_root){
        total=0;
        for(i=0;i<numnodes;i++)
        {
            //printf("Number of successes from %i were %i\n", i, back_ray[i]);
            total=total+back_ray[i];
        }
        double pi = ((double)total)/((double)n*numnodes) * 4;
        printf("PI = %1.5f\n", pi);
    }
    mpi_err = MPI_Finalize();

    return 0;
    //    pi = ((double)success)/((double)n) * 4;
    //  printf("PI = %1.5f\n", pi);
    //  printf("PI/4 = %1.5f\n", ((double)success/n));
}

int nsuccess(int n) 
{
    int success = 0;
    double pi = 0;
    for (int cnt = 0; cnt < n; cnt ++)
    {
        double x = rn();
        double y = rn();
        if ((x*x + y*y) <= 1.0)
        {
            success += 1;
        }
    }
    return success;
}

double rn()
{
    return  ((double)rand()/(double)(RAND_MAX-1.0)) ;
}

void init_it(int  *argc, char ***argv) {
    mpi_err = MPI_Init(argc,argv);
    mpi_err = MPI_Comm_size( MPI_COMM_WORLD, &numnodes );
    mpi_err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
}
