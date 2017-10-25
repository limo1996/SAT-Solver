import os
from randomCNF import CNFGenerator

path = "/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples"
# n_variables, n_clauses, examples, path
cnf = CNFGenerator(4,4,4,path)
cnf.saveCNF()


