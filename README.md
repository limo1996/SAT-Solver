# SAT-Solver
Parallel SAT Solver 

## Invoking
Usage:
```bash
./SAT-Solver <CNF_input_file> [-o <CNF_output_file>] [-p <format>] [-pa <format>]
```

There are 4 commands that have impact on solver's output:
- ``<CNF_input_file>`` -> path to the input file that has CNF format
- ``-o`` -> path to the output file. Output will not be printed into console.
- ``-p`` -> Basic extended output (includes values of variables). Two possible formats:
  - ``1`` -> Readable format
  - ``2`` -> Format for further processing (python script)
- ``-pa`` -> Extended output (includes values of variables and clauses). Two possible formats:
  - ``1`` -> Readable format
  - ``2`` -> Format for further processing (python script)

Example usage:
```bash
./SAT-Solver sat1.in -o sat1.out -pa 2
```
