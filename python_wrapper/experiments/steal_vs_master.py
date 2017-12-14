import os
import sys

sys.path.append('../')
from euler import EulerTester

import click
import matplotlib.pyplot as plt
import numpy as np

from abstract_experiment import AbstractExperiment
from utils import run_n_times, get_results, delete_files_in_folder, get_info, \
    Info, conf_95_mean


FOLDER = 'benchmark_formulas'
parent_parent = os.path.join(os.pardir, os.pardir)
CNF_FOLDER = os.path.join(parent_parent, os.path.join('cnfs', FOLDER))
EXECUTABLES = ['stealing', 'parallel']
REPETITIONS = 10
TIMEOUT = 120


class StealVsMaster(AbstractExperiment):
    def __init__(self, onEuler, nethz_username):
        super(StealVsMaster, self).__init__()
        self.nethz_username = nethz_username
        self.onEuler = onEuler

        if not self.onEuler:
            self.name = 'StealVsMaster'
        else:
            self.name = 'StealVsMasterEuler'

        # number of cores
        self.num_nodes = [2, 3, 4, 6, 9, 16, 20, 24, 28, 32, 38, 43, 48]

    def run_experiment(self):
        if self.onEuler:
            self.run_euler_experiment(self.nethz_username)
            if self.nethz_username == 'xxx':
                self.processs_euler_experiment()
            return

        self.re_init_data()
        files = sorted([os.path.join(CNF_FOLDER, f)
                        for f in os.listdir(CNF_FOLDER)
                        if os.path.isfile(os.path.join(CNF_FOLDER, f))
                        and f.endswith('.cnf')])
        for s in EXECUTABLES:
            exe = os.path.join(parent_parent,"./{}_main".format(s))
            delete_files_in_folder(CNF_FOLDER, 'time')
            self.data[s] = {}
            for f in files:
                print('main: {}  file: {}'.format(s, f))
                self.data[s][f] = {}
                self.data[s][f]['info'] = get_info(f).__dict__
                command = 'gtimeout {} mpirun -np 4 {} {}'.format(TIMEOUT, exe, f)
                run_n_times(command, REPETITIONS)
                timing_file = f[:-3] + 'time'
                times = get_results(timing_file)
                self.data[s][f]['time'] = times


    def run_euler_experiment(self, nethz_username):
        if nethz_username == 'xxx':
            return

        # overall runtime in minutes
        overall_runtime_minutes = 18 * 59
        tester = EulerTester(FOLDER, nethz_username, self.num_nodes, REPETITIONS,
                             TIMEOUT, overall_runtime_minutes)
        tester.run_test('..', 'bsub_script2.sh')

    def processs_euler_experiment(self):
        self.re_init_data()
        files = sorted([os.path.join(CNF_FOLDER, f)
                        for f in os.listdir(CNF_FOLDER)
                        if os.path.isfile(os.path.join(CNF_FOLDER, f))
                        and f.endswith('.cnf')])
        for s in EXECUTABLES:
            self.data[s] = {}
            for f in files:
                time_file = f.replace('.cnf', '_{}.time'.format(s))
                print('main: {}  file: {}'.format(s, time_file))

                self.data[s][f] = {}
                self.data[s][f]['info'] = get_info(f).__dict__
                times = get_results(time_file)
                self.data[s][f]['time'] = times

    def plot(self):
        markers = ['o', 'D']
        color = ['firebrick', 'teal']
        i = 0
        x_offsets = {}
        for s in EXECUTABLES:
            x_offsets = {}
            xs = []
            ys = []
            lower_error = []
            upper_error = []
            for k, v in self.data[s].iteritems():
                info = Info(**v['info'])
                data = []
                for t in v['time']:
                    data.append(t)
                if info.variables not in x_offsets:
                    x_offsets[info.variables] = -2.5
                x_offsets[info.variables] = x_offsets[info.variables] + 0.5
                xs.append(info.variables + x_offsets[info.variables])
                data1 = []
                for d in data:
                    data1.append(np.mean(d))
                mean = np.mean(data1)
                ys.append(mean)
                lower, upper = conf_95_mean(data1)
                lower_error.append(mean-lower)
                upper_error.append(upper-mean)
            plt.errorbar(xs, ys,
                         yerr=[lower_error, upper_error],
                         label=s,
                         fmt=markers[i],
                         color=color[i],
                         markerfacecolor='none')
            i = i + 1
        plt.xticks(x_offsets.keys(), x_offsets.keys())
        plt.title('Stealing vs Master-Worker, mean runtime with 95% confidence interval')
        plt.xlabel(
            'Approximate formula difficulty [formula size: number of variables]')
        plt.ylabel('Runtime [ms]')
        plt.legend(loc=2)
        plt.show()

def get_netz_username():
    nethz_username = raw_input('please enter your nethz_account name : ')
    return nethz_username

@click.command()
@click.option('--mode', type=click.Choice(['rerun-local',
                                           'no-rerun-local',
                                           'rerun-euler',
                                           'no-rerun-euler']),
              prompt='please specify a mode ('
                     'rerun-local | no-rerun-local | rerun-euler | no-rerun-euler)')
@click.option('--nethz_username', default='xxx')
def main(mode, nethz_username):
    euler = 'euler' in mode
    rerun = 'no' not in mode
    e = StealVsMaster(euler, nethz_username)
    if rerun:
        e.run_experiment()
        e.results_to_json()
    e.results_from_json()
    e.plot()

if __name__ == '__main__':
    main()
