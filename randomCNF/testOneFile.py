from z3 import *

from cnf_parser import CnfParser

cnf_parser = CnfParser()
cnf = cnf_parser.parse("sampleCNF.cnf")
cnf.to_z3()
s = Solver()
s.add(cnf.z3_formula)
result = s.check().r
print (result) # return 1 is sat, -1 is unsat