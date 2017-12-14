#!/usr/bin/env bash
cmake .
make

num_nodes=$(cat num_nodes.txt)
num_runs=$(cat num_runs.txt)
timeout=$(cat timeout.txt)

# yes, this is a nasty as it looks...
cd $(cat test_folder.txt)
rm -rf *.time
find . -name "*.comm" -type f -delete
find . -name "*.wait" -type f -delete
find . -name "*.time" -type f -delete

rm uuf50-01.cnf
rm uuf50-02.cnf

FILES=*.cnf

for nodes in ${num_nodes}
do
	for file in ${FILES}
	do
		for run in $(seq 1 $num_runs)
		do
			timeout "$timeout"s mpirun -np "$nodes" ../stealing_main "$file" > "$file"_stealing.out
		done
	done
done

for nodes in ${num_nodes}
do
	for file in ${FILES}
	do
		for run in $(seq 1 $num_runs)
		do
			timeout "$timeout"s mpirun -np "$nodes" ../parallel_main "$file" > "$file"_parallel.out
		done
	done
done

tar -cf time_measurements.tar *.time
tar -cf wait_measurements.tar *.wait
tar -cf comm_measurements.tar *.comm
echo "tar file (time_measurements.tar) with measurements created"
echo "tar file (wait_measurements.tar) with measurements created"
echo "tar file (comm_measurements.tar) with measurements created"
cd ../
