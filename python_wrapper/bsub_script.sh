#!/usr/bin/env bash
cmake .
make

num_nodes=$(cat num_nodes.txt)

# yes, this is a nasty as it looks...
cd $(cat test_folder.txt)
rm -rf *.time
FILES=*.cnf

for file in ${FILES}
do
    for run in 1 2 3 4 5 6 7 8 9 10
    do
        ../sequential_main "$file" -p 2 > "$file".out
    done
done

for nodes in ${num_nodes}
do
    for file in ${FILES}
    do
        for run in 1 2 3 4 5 6 7 8 9 10
        do
            mpirun -np "$nodes" ../parallel_main "$file" -p 2 > "$file".out
        done
    done
done
tar -cf time_measurements.tar *.time
echo "tar file (time_measurements.tar) with measurements created"

cd ../
