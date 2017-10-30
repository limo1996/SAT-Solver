#!/usr/bin/env bash
num_cores=4
minutes=1

echo "#########################################################"
echo "run_me_on_euler.sh is taking over"

echo "including necessary modules..."
module load cmake
module load open_mpi

echo "submitting compilation and run job to the batch system..."
bsub -W "$minutes" -n "$num_cores" < bsub_script.sh
# note: the actual compilation is handled in bsub_script.sh
# according to the documentation it's enough to specify the number of cores and so on here...

echo "job submitted, exiting"
echo "#########################################################"
