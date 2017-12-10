#!/usr/bin/env bash
cmake .
make

num_nodes=$(cat num_nodes.txt)
num_runs=$(cat num_runs.txt)
timeout=$(cat timeout.txt)

# yes, this is a nasty as it looks...
cd $(cat test_folder.txt)
rm -rf *.time *.wait *.comm
FILES=*.cnf

for file in ${FILES}
do
    for run in $(seq 1 $num_runs)
    do
        timeout "$timeout"s ../sequential_main "$file" > "$file".out
    done
done

for nodes in ${num_nodes}
do
    for file in ${FILES}
    do
        for run in $(seq 1 $num_runs)
        do
            timeout "$timeout"s mpirun -np "$nodes" ../parallel_main "$file" > "$file".out
        done
    done
done
tar -cf measurements.tar *.time *.wait *.comm
echo "tar file (measurements.tar) with measurements created"

cd ../
