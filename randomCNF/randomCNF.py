import random

# the return example is like 
# ['p', 'cnf', 5, 3, -2, 0, -1, -1, 0, -1, -1, -1, -1, -6, 0]

def randomCFN( n_variables, n_clauses):
	S = list()
	S = S + ['p', 'cnf', n_variables, n_clauses]
	print (S)
	for i in range(0, n_clauses):
		for j in range(0,random.randint(1, n_variables)):
			n = ((-1)**(random.randint(0,1)))*random.randint(1,n_variables)
			S = S + [n]
		S = S + [0]
	return S

print(randomCFN(3, 3))