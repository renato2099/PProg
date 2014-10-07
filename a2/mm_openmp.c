#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

static int const MAX_VALUE = 10;

int** ini_matrix(int** mat, int r, int c);
void print_matrix(int** mat, int r, int c);
int rn(int limit);
void fill_matrix(int** mat, int r, int c);
int** alloc_matrix(int r, int c);
void dealloc_matrix(int** mat);
int** mat_mult(int** mat1, int r1, int c1, int ** mat2, int r2, int c2);

int main( int argc, char *argv[] ) {
    int r, c, **mat1, **mat2, **mat3;
    clock_t ts = clock();
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
    //print_matrix(mat1, r, c);
    printf("\n");
    //print_matrix(mat2, r, c);
    printf("\n");
    mat3 = mat_mult(mat1, r, c, mat2, r, c);
    //print_matrix(mat3, r, c);
    clock_t te = clock();
    printf("Elapsed: %f seconds\n", (double)(te - ts) / CLOCKS_PER_SEC);
    dealloc_matrix(mat1);
    dealloc_matrix(mat2);
}

int** ini_matrix(int** mat, int r, int c) {
    for(int x = 0; x < r; x++)
        for (int y = 0; y < c; y++)
            mat[x][y] = 0;
    return mat;
}

void print_matrix(int** mat, int r, int c) {
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++)
            printf("%i ", mat[i][j]);
        printf("\n");
    }
}

int rn(int limit) {
    return rand()%limit;
}

void fill_matrix(int** mat, int r, int c) {
    for(int i = 0; i < r; i++)
        for(int j = 0; j < c; j++) {
            mat[i][j] = rn(MAX_VALUE);
        }
}

int** alloc_matrix(int r, int c) {
    //int **mat1=(int **)malloc(r*c*sizeof(int *));
    int **mat1=(int **)malloc(r*sizeof(int *));
    for(int i = 0; i < r; i++)
        mat1[i] = (int *)malloc(c*sizeof(int));
    return mat1;
}

void dealloc_matrix(int** mat) {
    free(mat);
}

int** mat_mult(int** mat1, int r1, int c1, int ** mat2, int r2, int c2) {
    int** mm = alloc_matrix(r1, c2);
    mm = ini_matrix(mm, r1, c2);
    int sum = 0, i, j, k;
    #pragma omp parallel private(i, j, k)
    {
        #pragma omp for
        for (i = 0; i < r1; i++) {
            for (j = 0; j < c2; j++) {
                k = 0;
                for(k = 0; k < r2; k++) {
                    mm[i][j] += mat1[i][k] * mat2[k][j];
                }
            }
        }
    }
    return mm;
}
