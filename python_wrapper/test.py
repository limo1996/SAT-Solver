import os
import subprocess
from datetime import datetime

from z3 import Solver, Not, And

from cnf_parser import CnfParser


class SolverError(Exception):
    pass


class Tester(object):
    def __init__(self, folder, parallel):
        folder = os.path.join(os.pardir, folder)
        self.folder = folder
        self.files = [os.path.join(folder, f)
                      for f in os.listdir(folder)
                      if os.path.isfile(os.path.join(folder, f))]
        self.cnf_parser = CnfParser()
        self.parallel = parallel
        self.solver = self.compile_and_create_solver()
        self.fail_count = 0

    def compile_and_create_solver(self):
        print('compiling...')
        cmake = 'cd ' + os.pardir + '; cmake . > /dev/null; cd python_wrapper'
        make = 'make -C ' + os.pardir + '> /dev/null'
        ret = subprocess.call(cmake, shell=True)
        if ret != 0:
            raise RuntimeError("cmake failed!")
        ret = subprocess.call(make, shell=True)
        if ret != 0:
            raise RuntimeError("make failed!")
        print('compilation done!')
        if self.parallel:
            return ParallelSolver()
        else:
            return SequentialSolver()

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
            try:
                if result is 1:
                    self.handle_sat_case(f, cnf)
                else:
                    self.handle_unsat_case(f)
            except SolverError:
                print('[crash] {0}'.format(f))
                self.fail_count += 1

        print('')
        correct_count = len(self.files) - self.fail_count
        count = len(self.files)
        print('{0} out of {1} tests successful'.format(correct_count, count))

    def handle_sat_case(self, input_file, cnf):
        output, runtime = self.solver.solve(input_file)
        correct_num_vars = len(output) is cnf.num_vars + 1
        output_correct = correct_num_vars and output[0].startswith('sat')
        if not output_correct:
            print('[fail]   {0} (sat)'.format(input_file))
            self.fail_count += 1
        else:
            if self._inputs_correct(output[1:], cnf):
                print('[ok]    {0} (sat, runtime: {1}ms)'.format(input_file, runtime))
            else:
                print('[fail]  {0} (sat)'.format(input_file))
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
        output, runtime = self.solver.solve(input_file)
        output_correct = len(output) == 1 and output[0].startswith('unsat')
        if output_correct:
            print('[ok]    {0} (unsat, runtime {1}ms)'.format(input_file, runtime))
        else:
            print('[fail]  {0} (unsat)'.format(input_file))
            self.fail_count += 1


class SequentialSolver(object):
    def __init__(self):
        cwd = os.path.join(os.getcwd(), os.path.pardir)
        self.executable = os.path.join(cwd, 'sequential_main')
        if not os.path.exists(self.executable):
            raise ValueError('The executable sequential_main does not exist!')

    def solve(self, input_file):
        """ Invokes the solver for a given input file

        :param input_file: the path to the input file
        :return: a list of lines that the solver did output to std_out
        """
        command = '{0} {1} -p 2 > out'.format(self.executable, input_file)
        start = datetime.now()
        ret = subprocess.call(command, shell=True,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
        stop = datetime.now()
        runtime = (stop - start).total_seconds() * 1000 # in miliseconds
        if ret != 0:
            raise SolverError("Solver did not return 0")

        f = open('out', 'r')
        return [line for line in f], runtime


class ParallelSolver(SequentialSolver):
    def __init__(self):
        self.num_cores = 4
        cwd = os.path.join(os.getcwd(), os.path.pardir)
        self.executable = os.path.join(cwd, 'parallel_main')
        if not os.path.exists(self.executable):
            raise ValueError('The executable parallel_main does not exist!')

        print('running in parallel on {0} cores'.format(self.num_cores))

    def solve(self, input_file):
        """ Invokes the solver for a given input file

        :param input_file: the path to the input file
        :return: a list of lines that the solver did output to std_out
        """
        command = 'mpirun -np {0} {1} {2} > out'.format(self.num_cores, self.executable, input_file)
        ret = subprocess.call(command, shell=True,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
        if ret != 0:
            raise SolverError("Solver did not return 0")

        f = open('out', 'r')
        runtime = -1
        return [line for line in f], runtime
