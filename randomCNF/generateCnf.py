from testSat import testCNF
from randomCNF import CNFGenerator
from test import Tester
import shutil
import os

# generate examples without considering probability of sat
# n_variables, n_clauses, examples, path

n_variables = 20
n_clauses = 200
n_examples = 10

path = '/home/ziwei/DPHPC/SAT-Solver/randomCNF/Examples' + str(n_variables) + '_' + str(n_clauses) + '_' +str(n_examples) # the folder save examples
# remove the old folder in case old examples exist when generate new examples
if os.path.exists(path):
	shutil.rmtree(path)

cnfGenerator = CNFGenerator(n_variables, n_clauses, n_examples, path) 
cnfGenerator.saveCNF()
# foldertester = Tester(path, False)
# foldertester.run_test()