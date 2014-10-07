#/bin/bash

echo "COMPILING" $1
C_PROG=$1
NAME_PROG=`echo $C_PROG | cut -d'.' -f 1`
mpicc -std=gnu99 $1 -o $NAME_PROG.out

echo "RUNNING MPI " $2 " THREADS"
mpirun -n $2 $NAME_PROG.out $3 $4
