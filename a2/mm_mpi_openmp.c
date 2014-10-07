#include "mpi.h"
#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int const MAX_VALUE = 10;
static int const mpi_root = 0;
int numnodes, myid, mpi_err;

void init_it(int  *argc, char ***argv);
int rn(int limit);
void fill_matrix(int** mat, int r, int c);
int** ini_matrix(int** mat, int r, int c);
int** alloc_matrix(int r, int c);
void dealloc_matrix(int** mat);
int** mat_mult(int**, int, int, int**, int, int);
int** mat_mult2(int**, int, int, int, int, int**, int, int);
void print_matrix(int** mat, int r, int c);
int* array_prod(int*, int**, int, int);

int main( int argc, char *argv[] ) {
    int r, c, **mat1, **mat2, **mat3, workers, start, rows;
    int *myray,*send_ray,*back_ray;
    clock_t ts, te;
    MPI_Status status;

    /***** initializing mpi ******/
    init_it(&argc,&argv);
    workers = numnodes - 1;

    /***** master thread initializing ******/
    if(myid == mpi_root){
        ts = clock();
        if (argc != 3) {
            printf("DEFAULT SIZE. 3X3\n");
            r = c = 3;
        } else {
            r = atoi(argv[1]);
            c = atoi(argv[2]);
        }
        //srand(time(NULL));
        srand(10);
        mat1 = alloc_matrix(r, c);
        mat2 = alloc_matrix(r, c);
        fill_matrix(mat1, r, c);
        fill_matrix(mat2, r, c);

        // if there are workers then do stuff
        if (workers > 0) {
            int wrow = r/workers, extra = r%workers;
            start = 0;
            rows = 0;
            // send information to processes
            for (int dest = 1; dest <= workers; dest++) {
                if (dest == workers) wrow += extra; 
                // vector size
                MPI_Send(&r, 1, MPI_INT, dest, 122, MPI_COMM_WORLD);
                MPI_Send(&c, 1, MPI_INT, dest, 123, MPI_COMM_WORLD);
                MPI_Send(&start, 1, MPI_INT, dest, 123, MPI_COMM_WORLD);
                MPI_Send(&wrow, 1, MPI_INT, dest, 123, MPI_COMM_WORLD);
                // vector 1
                MPI_Send(&mat1[start][0], wrow*c, MPI_INT, dest, 111, MPI_COMM_WORLD);
                // vector 2
                MPI_Send(&mat2[0][0], r*c, MPI_INT, dest, 112, MPI_COMM_WORLD);
                start += wrow;
            }
            mat3 = alloc_matrix(r, c);
            // receive data
            start = 0;
            int ss; 
            wrow = r/workers;
            for (int dest = 1; dest <= workers; dest++) {
                if (dest == workers) wrow += extra;
                //MPI_Recv(&ss, wrow*c, MPI_INT, dest, 123, MPI_COMM_WORLD, &status);
                MPI_Recv(&(mat3[start][0]), wrow*c, MPI_INT, dest, 123, MPI_COMM_WORLD, &status);
                //printf("\n start=%i ss=%i", start, ss);
                start += wrow;
            }
            //printf("---- RPTA ----\n");
            //print_matrix(mat3, r, c);
        } else {
            printf("\nNot enough workers.");
        }
    }
    /****** workers ******/
    else {
        int **v1, **v2, rr, cc, **r1;
        // vectors
        MPI_Recv(&rr, 1, MPI_INT, mpi_root, 122, MPI_COMM_WORLD, &status);
        MPI_Recv(&cc, 1, MPI_INT, mpi_root, 123, MPI_COMM_WORLD, &status);
        v1 = alloc_matrix(rr, cc);
        v2 = alloc_matrix(rr, cc);
        MPI_Recv(&(v1[0][0]), rr*cc, MPI_INT, mpi_root, 111, MPI_COMM_WORLD, &status);
        MPI_Recv(&(v2[0][0]), rr*cc, MPI_INT, mpi_root, 112, MPI_COMM_WORLD, &status);
        // which rows
        MPI_Recv(&start, 1, MPI_INT, mpi_root, 123, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, mpi_root, 123, MPI_COMM_WORLD, &status);
        // do computation
        r1 = mat_mult2(v1, start, rows, rr, cc, v2, rr, cc);
        //printf("\n");
        //print_matrix(r1, rows, cc);
        // send answer back
        //MPI_Send(&start, 1, MPI_INT, mpi_root, 123, MPI_COMM_WORLD);
        MPI_Send(&r1[0][0], rows*cc, MPI_INT, mpi_root, 123, MPI_COMM_WORLD);

        //MPI_Recv(v2, 1, MPI_INT, mpi_root, 112, MPI_COMM_WORLD, &status);

        /* send the local sums back to the root */
        //mpi_err = MPI_Gather(&total,    1,  MPI_INT,
        //         back_ray, 1,  MPI_INT,
        //        mpi_root,
        //        MPI_COMM_WORLD);
    }
    /***** master thread gathering *****/ 
    if(myid == mpi_root){
        te = clock();
        printf("Elapsed: %f seconds\n", (double)(te - ts) / CLOCKS_PER_SEC);
        dealloc_matrix(mat1);
        dealloc_matrix(mat2);
    }
    mpi_err = MPI_Finalize();
    return 0;
}

int* array_prod(int* v1, int** v2, int r, int c) {
    int* res = (int *)malloc(r*sizeof(int));
    for(int i = 0; i < r; i++) {
        int sum = 0;
        for(int j = 0; j < c; j++) {
            sum += v1[j]*v2[j][i];
        }
        res[i] = sum;
    }
    return res;
}

void init_it(int  *argc, char ***argv) {
    mpi_err = MPI_Init(argc,argv);
    mpi_err = MPI_Comm_size( MPI_COMM_WORLD, &numnodes );
    mpi_err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
}

int** mat_mult2(int** mat1, int start, int rows, int r1, int c1, int ** mat2, int r2, int c2) {
    int** mm = alloc_matrix(rows, c2);
    mm = ini_matrix(mm, rows, c2);
    int l = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < c2; j++) {
            for(int k = 0; k < r2; k++) {
                mm[l][j] += mat1[i][k] * mat2[k][j];
            }
        }
        l++;
    }
    return mm;
}
int** mat_mult(int** mat1, int r1, int c1, int ** mat2, int r2, int c2) {
    int** mm = alloc_matrix(r1, c2);
    mm = ini_matrix(mm, r1, c2);
    int i, j, k;
    #pragma omp parallel private(i, j, k)
    {
    #pragma omp for
    for ( i = 0; i < r1; i++)
        for ( j = 0; j < c2; j++)
            for( k = 0; k < r2; k++)
                mm[i][j] += mat1[i][k] * mat2[k][j];
    }
    return mm;
}

void dealloc_matrix(int** mat) {
    free(mat);
}

void print_matrix(int** mat, int r, int c) {
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++)
            printf("%i ", mat[i][j]);
        printf("\n");
    }
}

void fill_matrix(int** mat, int r, int c) {
    for(int i = 0; i < r; i++)
        for(int j = 0; j < c; j++) {
            mat[i][j] = rn(MAX_VALUE);
        }
}

int rn(int limit) {
    return rand()%limit;
}

int** ini_matrix(int** mat, int r, int c) {
    for(int x = 0; x < r; x++)
        for (int y = 0; y < c; y++)
            mat[x][y] = 0;
    return mat;
}

int** alloc_matrix(int r, int c) {
    //int **mat1=(int **)malloc(r*c*sizeof(int *));
    int *data = (int *)malloc(r*c*sizeof(int));
    int **array= (int **)malloc(r*sizeof(int*));
    for (int i=0; i<r; i++)
        array[i] = &(data[c*i]);
    return array;
}
