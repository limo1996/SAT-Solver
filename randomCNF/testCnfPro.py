from testSat import testCNF
from randomCNF import CNFGenerator
from test import Tester
import shutil
import os

path = '/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples' # the folder save examples
# remove the old folder in case old examples exist when generate new examples
if os.path.exists(path):
	shutil.rmtree(path)

pro = 0 # initial pro
n_variables = 3
n_clauses = 5
n_examples = 10


# generate examples with 0.5 sat
while pro != 0.5:
 	# n_variables, n_clauses, examples, path
 	# to get pro = 0.5 fast, consider n_clauses a bit larger than n_clauses
 	# it is always good to see the first few probabilities (print out), to have a general idea on 
 	# parameters setting
 	# for example, if set 5, 10, 100, the most probabilies seem are above 0.5
 	# change to 5, 14, 100, the probabilites are around 0.5, good!
 	# n_variables, n_clauses, examples, path
 	cnfGenerator = CNFGenerator(n_variables, n_clauses, n_examples, path) 
 	cnfGenerator.saveCNF()	
 	tester = testCNF(path)	
 	(count, count_file) = tester.run_test()
 	tester.print_result()
 	pro = float(count) / count_file
 	print(pro)

# check sequential result with z3 solver
foldertester = Tester(path, False)
foldertester.run_test()



# example output, with parameters 3, 5, 10
# 6 out of 10 tests sat
# 0.6
# 5 out of 10 tests sat
# 0.5
# compiling...
# compilation done!
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example5_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example4_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example10_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example7_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example1_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example3_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example9_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example2_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example6_var_3_cla_5.cnf
# [ok]    /home/ziwei/DPHPC/SAT-Solver/randomCNF/examples/example8_var_3_cla_5.cnf
