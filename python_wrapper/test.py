import multiprocessing
import os
import subprocess
from datetime import datetime

from z3 import Solver, Not, And

from cnf_parser import CnfParser


class SolverError(Exception):
    pass


class Tester(object):
    def __init__(self, folder, parallel, stealing, cdcl=False):
        folder = os.path.join(os.pardir, os.path.join('cnfs', folder))
        self.folder = folder
        self.files = []
        folders = [folder]
        for f in os.listdir(folder):
            if os.path.isdir(os.path.join(folder, f)):
                folders.append(os.path.join(folder, f))
        for fo in folders:
            self.files.extend(sorted([os.path.join(fo, f)
                                      for f in os.listdir(fo)
                                      if os.path.isfile(os.path.join(fo, f))
                                      and f.endswith('.cnf')]))
        self.cnf_parser = CnfParser()
        self.parallel = parallel
        self.stealing = stealing
        self.cdcl = cdcl
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
            return ParallelSolver(self.cdcl)
        elif self.stealing:
            return StealingSolver(self.cdcl)
        else:
            return SequentialSolver(self.cdcl)

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
                elif not self.stealing:
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
        correct_num_vars = len(output) == cnf.num_vars + 1
        if not correct_num_vars:
            correct_num_vars = len(output) == len(cnf.z3_vars) + 1
            if correct_num_vars:
                print('[warn]  {0} is inconsistent! #variables does not match cnf header!'.format(input_file))
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
    def __init__(self, cdcl):
        cwd = os.path.join(os.getcwd(), os.path.pardir)
        self.executable = os.path.join(cwd, 'sequential_main')
        self.cdcl = cdcl
        if not os.path.exists(self.executable):
            raise ValueError('The executable sequential_main does not exist!')

    def solve(self, input_file):
        """ Invokes the solver for a given input file

        :param input_file: the path to the input file
        :return: a list of lines that the solver did output to std_out
        """
        args = ''
        if self.cdcl:
            args = '-s CDCL'
        command = '{0} {1} {2} 1> out'.format(self.executable, args, input_file)
        start = datetime.now()
        ret = subprocess.call(command, shell=True,
                              stdout=subprocess.PIPE,
                              stderr=None)
        stop = datetime.now()
        runtime = (stop - start).total_seconds() * 1000 # in miliseconds
        if ret != 0:
            raise SolverError("Solver did not return 0")

        f = open('out', 'r')
        return [line for line in f], runtime


class ParallelSolver(SequentialSolver):
    def __init__(self, cdcl):
        self.num_cores = max(4, multiprocessing.cpu_count())
        self.cdcl = cdcl
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
        args = ''
        if self.cdcl:
            args = '-local-cdcl 2'
        command = 'mpirun -np {0} {1} {2} {3} 1> out'.format(self.num_cores,
                                                             self.executable,
                                                             args,
                                                             input_file)
        start = datetime.now()
        ret = subprocess.call(command, shell=True,
                              stdout=subprocess.PIPE,
                              stderr=None)
        stop = datetime.now()
        runtime = (stop - start).total_seconds() * 1000 # in miliseconds
        if ret != 0:
            raise SolverError("Solver did not return 0")

        f = open('out', 'r')
        return [line for line in f], runtime


class StealingSolver(ParallelSolver):
    def __init__(self, cdcl):
        self.num_cores = max(4, multiprocessing.cpu_count())
        self.cdcl = cdcl
        cwd = os.path.join(os.getcwd(), os.path.pardir)
        self.executable = os.path.join(cwd, 'stealing_main')
        if not os.path.exists(self.executable):
            raise ValueError('The executable stealing_main does not exist!')

        print('running in parallel on {0} cores'.format(self.num_cores))
