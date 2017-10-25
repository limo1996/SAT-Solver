import os
from z3 import Solver
from cnf_parser import CnfParser

class testCNF():
	def __init__(self, folder):
		folder = os.path.join(os.pardir, folder)
		self.folder = folder
		self.files = [os.path.join(folder, f)
						for f in os.listdir(folder)
						if os.path.isfile(os.path.join(folder, f))]
		self.cnf_parser = CnfParser()
		self.count = 0

	def run_test(self):
		for f in self.files:
			cnf = self.cnf_parser.parse(f)
			cnf.to_z3()
			s = Solver()
			s.add(cnf.z3_formula)
			result = s.check().r
			#print(result)
			if result is 1:
				self.count += 1
		count_file = len(self.files)
		self.print_result()
		return(self.count,count_file)

	def print_result(self):
		count = self.count
		count_file = len(self.files)
		print('{0} out of {1} tests sat'.format(self.count, count_file))


# for double check the final result of saved samples are with 0.5 sat and 0.5 unsat
# path = "/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples"
# test = testCNF(path)
# test.run_test()