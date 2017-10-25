import random

def randomCFN(n_variables, n_clauses):
    s = list()
    s = s + ['p', 'cnf', str(n_variables), str(n_clauses)]
    for i in range(0, n_clauses):
        s_len_pre = len(s)
        for j in range(1, n_variables + 1): # randomly choose each variable in a clause
            if random.randint(0, 1) == 0:
                n = ((-1) ** (random.randint(0, 1))) * j
                s = s + [str(n)]
        if s_len_pre == len(s): # if no variable is slected in the preious step, force to have at lease one variable in the line
            n = ((-1) ** (random.randint(0, 1))) * random.randint(1,n_variables)
            s = s + [str(n)]        
        s = s + [str(0)]
    return s

# random.seed(2)
#n_variables = 2
#n_clauses = 3

s = randomCFN(n_variables, n_clauses)
print(s)

file = open("sampleCNF.cnf", "w")
i = 1
for ele in s:
    if i <= 3:
        file.write(ele + ' ')
        i = i + 1
    elif i == 4:
        file.write(ele + '\n')
        i = i +1
    elif ele != '0':
        file.write(ele+' ')
    else:
        file.write(ele+'\n')
file.close()


