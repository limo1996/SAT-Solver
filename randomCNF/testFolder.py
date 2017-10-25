# from test import Tester

# tester = Tester("/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples")
# tester.run_test()

from randomCNF import CNFGenerator

cnfGenerator = CNFGenerator()
cnfGenerator.n_variables = 3
cnfGenerator.n_clauses = 5
cnfGenerator.randomCNF()
