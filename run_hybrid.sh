#/bin/bash

echo "COMPILING" $1
C_PROG=$1
NAME_PROG=`echo $C_PROG | cut -d'.' -f 1`
mpicc -std=gnu99 -fopenmp $1 -o $NAME_PROG.out

OPENMP=$(( $2/2 ))
echo "RUNNING WITH " $2 " MPI PROCESSES + " $OPENMP " OMP THREADS"

export OMP_NUM_THREADS=$OPENMP

mpirun -n $2 $NAME_PROG.out $3 $4
