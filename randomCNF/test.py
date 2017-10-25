import os
import subprocess

from z3 import Solver, Not, And

from cnf_parser import CnfParser


class Tester(object):
    def __init__(self, folder):
        folder = os.path.join(os.pardir, folder)
        self.folder = folder
        self.files = [os.path.join(folder, f)
                      for f in os.listdir(folder)
                      if os.path.isfile(os.path.join(folder, f))]
        self.cnf_parser = CnfParser()
        self.solver = self.compile_and_create_solver()
        self.fail_count = 0

    @staticmethod
    def compile_and_create_solver():
        cpp_path = os.path.join(os.pardir, 'dummy_solver.cpp')
        cpp_o_path = os.path.join(os.pardir, 'solver.cpp.o')
        command = 'g++ --std=c++11 {0} -o {1}'.format(cpp_path, cpp_o_path)
        ret = subprocess.call(command, shell=True)
        if ret != 0:
            raise RuntimeError("c++ compilation failed!")
        return ParallelSolver()

    def print_file_list(self):
        for f in self.files:
            print(f)

    def run_test(self):
        for f in self.files:
            cnf = self.cnf_parser.parse(f)
            cnf.to_z3()
            s = Solver()
            s.add(cnf.z3_formula)
            result = s.check().r
            print(result)
            if result is 1:
                self.handle_sat_case(f, cnf)
            else:
                self.handle_unsat_case(f)
        print('')
        correct_count = len(self.files) - self.fail_count
        count = len(self.files)
        print('{0} out of {1} tests successful'.format(correct_count, count))

    def handle_sat_case(self, input_file, cnf):
        output = self.solver.solve(input_file)
        correct_num_vars = len(output) is cnf.num_vars + 1
        output_correct = correct_num_vars and output[0].startswith('sat')
        if not output_correct:
            print('[fail]   {0}'.format(input_file))
        else:
            if self._inputs_correct(output[1:], cnf):
                print('[ok]   {0}'.format(input_file))
            else:
                print('[fail] {0}'.format(input_file))
                self.fail_count += 1

    def _inputs_correct(self, input_lines, cnf):
        z3_encoded_inputs = []
        for l in input_lines:
            var_name = int(l.split(' ')[0])
            # TODO: probably a regex would be more readable here, but...
            var_neg = l.split(' ')[1].split('\n')[0] == 'f'
            if var_neg:
                z3_encoded_inputs.append(Not(cnf.z3_vars[var_name]))
            else:
                z3_encoded_inputs.append(cnf.z3_vars[var_name])
        inputs = And(*z3_encoded_inputs)
        s = Solver()
        s.add(And(cnf.z3_formula, inputs))
        result = s.check().r
        if result is 1:
            return True
        else:
            return False

    def handle_unsat_case(self, input_file):
        output = self.solver.solve(input_file)
        output_correct = len(output) == 1 and output[0].startswith('unsat')
        if output_correct:
            print('[ok]   {0}'.format(input_file))
        else:
            print('[fail] {0}'.format(input_file))
            self.fail_count += 1


class ParallelSolver(object):
    def __init__(self):
        self.num_cores = 1
        cwd = os.path.join(os.getcwd(), os.path.pardir)
        self.executable = os.path.join(cwd, 'solver.cpp.o')
        if not os.path.exists(self.executable):
            raise ValueError('The executable solver.cpp.o does not exist!')

    def solve(self, input_file):
        """ Invokes the solver for a given input file

        :param input_file: the path to the input file
        :return: a list of lines that the solver did output to std_out
        """
        command = '{0} < {1} > out'.format(self.executable, input_file)
        ret = subprocess.call(command, shell=True)
        if ret != 0:
            # return the error code if the solver crashed
            return ret

        f = open('out', 'r')
        return [line for line in f]
