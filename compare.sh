#/bin/bash
if [ "$#" -ne 4 ]; then
        echo "Usage <filePattern> <numberThreads> <rows> <cols>"
        exit
fi
FILE=$1
NTHREADS=$2
ROWS=$3
COLS=$4
MPI_EXT="_mpi"
SEQ_EXT="_seq"
OPENMP_EXT="_openmp"
HYBRID_EXT="_mpi_openmp"

sh run_seq.sh $FILE$SEQ_EXT".c" $ROWS $COLS
sh run_mpi.sh $FILE$MPI_EXT".c" $NTHREADS $ROWS $COLS
sh run_openmp.sh $FILE$OPENMP_EXT".c" $NTHREADS $ROWS $COLS
sh run_hybrid.sh $FILE$HYBRID_EXT".c" $NTHREADS $ROWS $COLS
