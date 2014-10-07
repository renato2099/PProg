#/bin/bash

echo "COMPILING" $1
C_PROG=$1
NAME_PROG=`echo $C_PROG | cut -d'.' -f 1`
gcc -std=gnu99 -fopenmp $1 -o $NAME_PROG.out

export OMP_NUM_THREADS=$2

echo "RUNNING OPENMP " $2 " THREADS"
./$NAME_PROG.out $3 $4
