#!/usr/bin/env bash
num_nodes=$(cat num_nodes.txt)
minutes=$(cat overall_runtime_minutes.txt)

echo "#########################################################"
echo "run_me_on_euler.sh is taking over"

echo "including necessary modules..."
module load cmake
module load open_mpi

echo "node counts: $num_nodes"
n=0
for nodes in ${num_nodes}
do
    n="$nodes"
done
echo "choosing highest node count: $n"

echo "submitting compilation and run job to the batch system..."
bsub -W "$minutes" -n "$nodes" < bsub_script.sh

# note: the actual compilation is handled in bsub_script.sh

echo "job submitted, exiting"
echo "#########################################################"
