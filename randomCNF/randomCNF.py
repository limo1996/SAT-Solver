import random

# the return example is like 
# ['p', 'cnf', 5, 3, 1, 2, -3, 5, 0, 3, -5, 0, 3, 0]


def randomCFN( n_variables, n_clauses):

	S = list()
	S = S + ['p', 'cnf', n_variables, n_clauses]
	print (S)
	for i in range(0, n_clauses):
		for j in range(1,n_variables+1):
			if random.randint(0,1) == 0:
				n = ((-1)**(random.randint(0,1)))*j
				S = S + [n]
		S = S + [0]
	return S

#random.seed(2)
#print(randomCFN(5, 3))