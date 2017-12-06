from collections import namedtuple
import re

from z3 import *


class CnfParser(object):
    def parse(self, path):
        f = open(path, 'r')
        cnf = None
        for line in f:
            line_str = str(line)
            if line_str.startswith('c'):
                continue
            elif line_str.startswith('p'):
                reg = re.match(r'p cnf (\S+)(\s*)(\S+)', line_str)
                cnf = CnfIntermediateRep(int(reg.group(1)), int(reg.group(3)))
            elif len(line_str) > 0:
                self._parse_line(cnf, line)
        return cnf

    def _parse_line(self, cnf, line):
        numbers = line.split(' ')[:-1]
        numbers = list(filter(lambda _n: _n != '', numbers))
        literals = []
        for n in numbers:
            n_int = int(n)
            if n_int > 0:
                literals.append(Literal(n_int, False))
            else:
                literals.append(Literal(-n_int, True))
        cnf.add_clause(Clause(literals))


# TODO: style-wise it would be nicer to use a builder pattern here, but...
class CnfIntermediateRep(object):
    def __init__(self, num_vars, num_clause):
        self.num_vars = num_vars
        self.num_clause = num_clause
        self.clauses = []
        self.z3_vars = {}
        self.z3_formula = None

    def add_clause(self, clause):
        self.clauses.append(clause)

    def to_z3(self):
        self.z3_vars = self._collect_variables(self.clauses)
        conjuncts = []
        for c in self.clauses:
            def _literal_to_z3(l):
                if l.negated:
                    return Not(self.z3_vars[l.atom])
                else:
                    return self.z3_vars[l.atom]

            disjuncts = list(map(_literal_to_z3, c.literals))
            conjuncts.append(Or(*disjuncts))
        self.z3_formula = And(*conjuncts)

    @staticmethod
    def _collect_variables(clauses):
        vars = {}
        for c in clauses:
            for l in c.literals:
                if l.atom not in vars:
                    vars[l.atom] = Bool('p' + str(l.atom))
        return vars


Clause = namedtuple('Clause', 'literals')
Literal = namedtuple('Literal', 'atom negated')
