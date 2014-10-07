#/bin/bash

echo "COMPILING" $1
C_PROG=$1
NAME_PROG=`echo $C_PROG | cut -d'.' -f 1`
gcc -std=gnu99 $1 -o $NAME_PROG.out

echo "RUNNING SEQUENTIALLY"
./$NAME_PROG.out $2 $3
