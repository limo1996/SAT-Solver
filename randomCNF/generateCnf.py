from testSat import testCNF
from randomCNF import CNFGenerator
from test import Tester
import shutil
import os

# generate examples without considering probability of sat
# n_variables, n_clauses, examples, path

path = '/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples' # the folder save examples
# remove the old folder in case old examples exist when generate new examples
if os.path.exists(path):
	shutil.rmtree(path)

# parameters
n_variables = 5
n_clauses = 10
n_examples = 10

cnfGenerator = CNFGenerator(n_variables, n_clauses, n_examples, path) 
cnfGenerator.saveCNF()
foldertester = Tester(path, False)
foldertester.run_test()