from __future__ import print_function

import os
import random
import sys
from collections import namedtuple

from z3 import Solver

from cnf_parser import CnfIntermediateRep, Literal, Clause


class ParameterChooser(object):
    def __init__(self, sat_prob, clause_size, sample_size, accepted_variance, prev):
        self.sat_prob = sat_prob
        self.clause_size = clause_size
        self.sample_size = sample_size
        self.accepted_variance = accepted_variance
        self.prev = prev

    def find_parameters(self, num_vars):
        num_clauses = self.prev  # initial clause size
        generator = CNFGenerator('', self.clause_size)
        sample_sat_count = -100
        while abs(sample_sat_count - self.sample_size*self.sat_prob) > \
                self.accepted_variance:
            sample_sat_count = 0
            for i in range(self.sample_size):
                cnf_encoded = generator.randomCNF(num_vars, num_clauses)
                cnf = generator._formula_to_cnf(num_vars, cnf_encoded)
                sat = generator._is_sat(cnf)
                if sat:
                    sample_sat_count = sample_sat_count + 1
            if sample_sat_count > self.sample_size*self.sat_prob:
                # formula too easy, increase num_clauses
                num_clauses = num_clauses + 2
                print('increasing num_clauses to {}'.format(num_clauses),
                      file=sys.stderr)
            else:
                # formula too hard, decrease num_clauses
                num_clauses = num_clauses - 2
                print('decreasing num_clauses to {}'.format(num_clauses),
                      file=sys.stderr)
            print('sat prob %1.2f' % (float(sample_sat_count)/float(self.sample_size)),
                  file=sys.stderr)
        self.prev = num_clauses
        print("num_vars:{0} sat_prob:{1} num_clauses:{2}".format(num_vars,
                                                                 self.sat_prob,
                                                                 num_clauses))
        return Paremeters(num_vars=num_vars, num_clauses=num_clauses)


Paremeters = namedtuple('Parameters', 'num_vars num_clauses')


class CNFGenerator(object):
    def __init__(self, path, clause_size):
        self.path = path
        self.clause_size = clause_size

    def _generate_formula(self, num_vars, num_clauses, clause_size, sign_prob):
        """
        Generates a random formula with the provided properties

        :return: a list of lists of ints that represent the formula
        """
        all_variables = range(1, num_vars+1)
        formula = []
        for i in range(num_clauses):
            clause = random.sample(all_variables, clause_size)
            clause_with_sign = []
            for j in range(0, clause_size):
                if random.randint(0, 1/sign_prob) <= 1:
                    clause_with_sign.append(-clause[j])
                else:
                    clause_with_sign.append(clause[j])
            formula.append(clause_with_sign)
        return self._let_every_variable_appear_once(num_vars, formula)

    @staticmethod
    def _let_every_variable_appear_once(num_vars, formula):
        """
        adds variables that happen to not occur in the formula to the formula

        :param formula: the encoded formula list of lists of integers
        """
        appears = [False] * (num_vars + 1)
        for clause in formula:
            for variable in clause:
                v = variable if variable > 0 else -variable
                appears[v] = True
        for i in range(1, num_vars+1):
            if not appears[i]:
                formula.append([i])
        return formula

    @staticmethod
    def _formula_to_string(formula):
        s = []
        for clause in formula:
            clause_str = list(map(lambda c: str(c), clause))
            clause_str.append('0')
            s.append(clause_str)
        return s

    @staticmethod
    def _formula_to_cnf(num_vars, encoded_formula):
        """
        transforms an encoded formula in a CnfIntermediateRep object
        """
        formula = CnfIntermediateRep(num_vars=num_vars,
                                     num_clause=len(encoded_formula))
        for encoded_clause in encoded_formula:
            literals = []
            for encoded_variable in encoded_clause:
                if encoded_variable < 0:
                    literals.append(Literal(-encoded_variable, False))
                else:
                    literals.append(Literal(encoded_variable, True))
            formula.add_clause(Clause(literals))
        return formula

    @staticmethod
    def _is_sat(cnf):
        cnf.to_z3()
        s = Solver()
        s.add(cnf.z3_formula)
        result = s.check().r
        return result is 1

    def randomCNF(self, n_variables, n_clauses):
        formula = self._generate_formula(n_variables, n_clauses,
                                         clause_size=self.clause_size,
                                         sign_prob=0.5)
        return formula

    def generate_cnfs_and_save(self, num_cnfs, n_variables, n_clauses, clause_size):
        path = self.path
        if not os.path.exists(path):
            os.makedirs(path)

        for n in range(0, num_cnfs):
            encoded_formula = self.randomCNF(n_variables, n_clauses)
            cnf = self._formula_to_cnf(n_variables, encoded_formula)
            sat = self._is_sat(cnf)
            sat_string = 'is-sat' if sat else 'is-unsat'
            filename = str(clause_size) + '-sat_var_'\
                       + str(n_variables) + '_cla_' +str(n_clauses) + '_'\
                       + sat_string + '_case_' + str(n)+ '.cnf'
            with open(os.path.join(path,filename), 'wb') as file:
                file.write('c {}\n'.format(sat_string))
                file.write('p cnf {0} {1}\n'.format(n_variables, n_clauses))
                formula_strings = self._formula_to_string(encoded_formula)
                for clause_strings in formula_strings:
                    line = ' '.join(clause_strings)
                    file.write(line)
                    file.write('\n')
                file.close()


if __name__ == '__main__':
    num_variables = [100, 150, 200, 250, 300]
    sat_prob = 0.5
    clause_size = 3
    sample_size = 100
    accepted_variance = 4
    num_cnfs = 20
    folder_name = '../3-sat_instances_large'
    prev = 0
    for num_vars in num_variables:
        parameterChoose = ParameterChooser(sat_prob, clause_size,
                                           sample_size, accepted_variance, prev)
        params = parameterChoose.find_parameters(num_vars)
        prev = parameterChoose.prev
        print('found good parameters, writing {} formulas to file'
              .format(num_cnfs), file=sys.stderr)
        cnf_generator = CNFGenerator(folder_name, clause_size=clause_size)
        cnf_generator.generate_cnfs_and_save(num_cnfs, params.num_vars,
                                             params.num_clauses, clause_size)
