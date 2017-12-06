#!/bin/sh

TimeOut=240
Files="$1/*.cnf"
TargetFolder="$1/solvable_in_time_$TimeOut_cdcl"
SolverCommand="./sequential_main -s CDCL"

mkdir $TargetFolder
for f in $Files
do
    echo "Solving $f..."
    base=$(basename "$f")
    timeout $TimeOut bash -c "$SolverCommand $f > /dev/null; cp $f $TargetFolder/$base; echo solvable!"
done
