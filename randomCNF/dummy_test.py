# import random
# 
# s1 = ['2','1', '3']
# s2 = ['1','1', '2', '2','3','4','1']
# for x in s1:
# 	s2.remove(x)
# 	print (s2)


from testSat import testCNF
from randomCNF import CNFGenerator
from test import Tester

# path = "/home/ziwei/DPHPC/SAT-Solver/randomCNF/examples" # the folder save examples
# pro = 0 # initial pro
# 
# while pro != 0.5:
# 	# n_variables, n_clauses, examples, path
# 	cnfGenerator = CNFGenerator(3, 5, 10, path) 
# 	cnfGenerator.saveCNF()	
# 	tester = testCNF(path)	
# 	(count, count_file) = tester.run_test()
# 	pro = float(count) / count_file
# 	print(pro)


# the final examples in the folder are with 0.5 sat and 0.5 unsat

path = "/home/ziwei/DPHPC/SAT-Solver/integration_tests"
foldertester = Tester(path)
foldertester.run_test()




#for y in range(1,5):
#	if str(y) not in s1 and str(-y) not in s1:
#		print(y)
#		sign = (-1) ** (random.randint(0, 1))
#		s1 = s1 + [str(sign * y)]
#
#print(s1)
