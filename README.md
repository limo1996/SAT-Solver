# Parallel SAT Solver
Semester project for Design of Parallel and High Performance Computing class at ETHZ. Includes two communication models for DPLL algorithm that could be combined with local CDCL. If you want to find out more about communication models or other techniques that we have used please refer to our final [report](https://github.com/limo1996/SAT-Solver/blob/master/report/report.pdf).

## Build
Please follow these steps in order to successfully compile the source code:
1. Open a terminal and navigate to the directory where you want to store the repository
2. ```git clone https://github.com/limo1996/SAT-Solver.git```
3. ```cd SAT-Solver```
4. ```cmake .```
5. ```make```
6. Three executables (*./sequential_main*, *./parallel_main*, *./stealing_main*) should be generated

## Invoking
Three executables are available: 
1. The sequential version of solver is named *./sequential_main*
2. The parallel version that uses master-worker communication pattern is named *./parallel_main*
3. The parallel version that uses work stealing communication pattern is named *./stealing_main* and from now we will call it Stealing version

## Usage
### Usage of sequential version:
```
./sequential_main [-s CDCL/DPLL] <CNF_input_file> 
```

Example usage of sequential version:
```bash
./sequential_main -s CDCL cnfs/benchmark_formulas/flat75-4.cnf
```

### Usage of parallel version:
```
./parallel_main [-local-cdcl branching_factor : int] <CNF_input_file>
```

Example usage of parallel version:
```
./parallel_main -local-cdcl 3 cnfs/benchmark_formulas/ais8.cnf
```
### Usage of stealing version:
```
./stealing_main [-local-cdcl branching_factor : int] <CNF_input_file>
```

Example usage of stealing version: 
```
./stealing_main -local-cdcl 2 cnfs/benchmark_formulas/anomaly.cnf
```

## Testing
We have developed a testing python wrapper whose documentation can be found [here](python_wrapper/README.md)
The python wrapper can also be used to run the solver on the Euler Supercompute Cluster of ETH.

## Results
  * Final report in pdf format can be found [here](https://github.com/limo1996/SAT-Solver/blob/master/report/report.pdf).
  * Performance graphs can be found [here](https://github.com/limo1996/SAT-Solver/tree/master/results)
  and [here](report/figures).
