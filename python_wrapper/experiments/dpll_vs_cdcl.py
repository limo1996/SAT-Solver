import os

import math
import click
import matplotlib.pyplot as plt
import numpy as np

from abstract_experiment import AbstractExperiment
from utils import run_n_times, get_results, delete_files_in_folder, conf_95_mean

FOLDER = 'benchmark_formulas'
parent_parent = os.path.join(os.pardir, os.pardir)
CNF_FOLDER = os.path.join(parent_parent, os.path.join('cnfs', FOLDER))
EXECUTABLE = './sequential_main'
REPETITIONS = 4
TIMEOUT = 120

HISTO_LOWER = 0
HISTO_UPPER = 8000
HISTO_STEP_SIZE = 500


class DpllVsCdcl(AbstractExperiment):
    def __init__(self):
        super(DpllVsCdcl, self).__init__()
        self.name = 'DpllVsCdcl'

    def run_experiment(self):
        self.re_init_data()
        files = sorted([os.path.join(CNF_FOLDER, f)
                        for f in os.listdir(CNF_FOLDER)
                        if os.path.isfile(os.path.join(CNF_FOLDER, f))
                        and f.endswith('.cnf')])
        exe = os.path.join(parent_parent, EXECUTABLE)
        for s in ['DPLL', 'CDCL']:
            delete_files_in_folder(CNF_FOLDER, 'time')
            self.data[s] = {}
            for f in files:
                print('solver: {}  file: {}'.format(s, f))
                self.data[s][f] = {}
                command = 'timeout {} {} -s {} {}'.format(TIMEOUT, exe, s, f)
                run_n_times(command, REPETITIONS)
                timing_file = f[:-3] + 'time'
                times = get_results(timing_file)
                self.data[s][f]['time'] = times

    def plot(self):
        self._plot_scatter()
        self._plot_speedup()

    def _plot_speedup(self):
        speedup = []
        dpll_times = {}
        cdcl_times = {}
        for s in ['DPLL', 'CDCL']:
            for k, v in self.data[s].iteritems():
                mean = np.mean(np.array(v['time'], dtype=float))
                if not math.isnan(mean):
                    if s == 'DPLL':
                        dpll_times[k] = mean
                    else:
                        cdcl_times[k] = mean
        for k, v in dpll_times.iteritems():
            if k in cdcl_times:
                d = dpll_times[k]
                c = cdcl_times[k]
                speedup.append(d / c)
        plt.hist(speedup, color='teal')
        plt.xlabel('Speedup of CDCL, baseline DPLL')
        plt.ylabel('Count')
        plt.show()

    def _plot_scatter(self):
        markers = ['o', 'D']
        color = ['firebrick', 'teal']
        i = 0
        x_offsets = {}
        plt.figure(figsize=(6, 4))
        for s in ['DPLL', 'CDCL']:
            x_offsets = {}
            xs = []
            ys = []
            lower_error = []
            upper_error = []
            for k, v in self.data[s].iteritems():
                data = []
                for t in v['time']:
                    data.append(t)
                xs.append(k)
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
        plt.xticks(x_offsets.keys()[::2], x_offsets.keys()[::2])
        plt.title('DPLL vs CDCL, mean runtime with 95% confidence interval')
        plt.xlabel(
            'Approximate formula difficulty [formula size: number of variables]')
        plt.ylabel('Runtime [ms]')
        plt.legend(loc=2)
        plt.gcf().savefig('{}/dpll_vs_cdcl_scatter.pdf'.format(self.figures_folder),
                    format='pdf')
        plt.show()


@click.command()
@click.option('--rerun/--no-rerun', default=False,
              help='Rerun the experiments')
def main(rerun):
    e = DpllVsCdcl()
    if rerun:
        e.run_experiment()
        e.results_to_json()
    e.results_from_json()
    e.plot()


if __name__ == '__main__':
    main()
