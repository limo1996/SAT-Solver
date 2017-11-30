import os

import click
import matplotlib.pyplot as plt
import numpy as np

from abstract_experiment import AbstractExperiment
from utils import run_n_times, get_results, delete_files_in_folder, get_info, \
    Info, conf_95_mean

FOLDER = 'steal_vs_master'
parent_parent = os.path.join(os.pardir, os.pardir)
CNF_FOLDER = os.path.join(parent_parent, os.path.join('cnfs', FOLDER))
EXECUTABLES = ['stealing_main', 'parallel_main']
REPETITIONS = 20
TIMEOUT = 10


class StealVsMaster(AbstractExperiment):
    def __init__(self):
        super(StealVsMaster, self).__init__()
        self.name = 'StealVsMaster'

    def run_experiment(self):
        self.re_init_data()
        files = sorted([os.path.join(CNF_FOLDER, f)
                        for f in os.listdir(CNF_FOLDER)
                        if os.path.isfile(os.path.join(CNF_FOLDER, f))
                        and f.endswith('.cnf')])
        for s in EXECUTABLES:
            exe = os.path.join(parent_parent,"./{}".format(s))
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
                mean = np.mean(data)
                ys.append(mean)
                lower, upper = conf_95_mean(data)
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


@click.command()
@click.option('--rerun/--no-rerun', default=False,
              help='Rerun the experiments')
def main(rerun):
    e = StealVsMaster()
    if rerun:
        e.run_experiment()
        e.results_to_json()
    e.results_from_json()
    e.plot()


if __name__ == '__main__':
    main()
