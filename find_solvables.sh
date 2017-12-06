#!/bin/sh

TimeOut=30
Files="$1/*.cnf"
TargetFolder="$1/solvable_in_time_${TimeOut}"
SolverCommand="mpirun -np 12 ./parallel_main"

mkdir $TargetFolder
for f in $Files
do
    echo "Solving $f..."
    base=$(basename "$f")
    timeout $TimeOut bash -c "$SolverCommand $f > /dev/null; cp $f $TargetFolder/$base; echo solvable!"
done
