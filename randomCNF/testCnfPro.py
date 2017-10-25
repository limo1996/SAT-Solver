from testSat import testCNF
from randomCNF import CNFGenerator

path = "/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples" # the folder save examples
pro = 0 # initial pro

while pro != 0.5:
	# n_variables, n_clauses, examples, path
	cnfGenerator = CNFGenerator(5, 10, 10, path) 
	cnfGenerator.saveCNF()	
	tester = testCNF(path)	
	(count, count_file) = tester.run_test()
	pro = float(count) / count_file
	print(pro)

# the final examples in the folder are with 0.5 sat and 0.5 unsat