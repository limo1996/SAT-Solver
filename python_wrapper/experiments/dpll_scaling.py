import os
import sys

import click
import numpy as np
import matplotlib.pyplot as plt

from abstract_experiment import AbstractExperiment
from utils import get_netz_username, parse_into_dict, conf_95_mean

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from euler import EulerTester


class DpllScaling(AbstractExperiment):
    def __init__(self):
        super(DpllScaling, self).__init__()
        self.name = 'DpllScaling'

    def run_experiment(self):
        test_folder = 'benchmark_formulas'
        # number of cores
        num_nodes = [1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 20, 24, 32, 48]
        # number of runs per formula
        num_runs = 1
        # timeout per formula in seconds
        timeout = 120
        # overall runtime in minutes
        overall_runtime_minutes = 20
        nethz_username = get_netz_username()
        tester = EulerTester(test_folder, nethz_username, num_nodes, num_runs,
                             timeout, overall_runtime_minutes)
        tester.run_test(basepath='..')

    def process_euler_experiment(self):
        self.data = parse_into_dict('measurements.tar')

    def plot(self):
        self._runtime_plot()
        self._speedup_plot()

    def _runtime_plot(self):
        for f, v in self.data.items():
            xs = []
            ys = []
            lowers = []
            uppers = []
            if 'seq' in v:
                xs.append(1)
                mean = np.mean(np.array(v['seq']['time']).flatten())
                ys.append(mean)
                lower, upper = conf_95_mean(np.array(v['seq']['time']).flatten())
                lowers.append(mean - lower)
                uppers.append(upper - mean)
            for cores, v_ in v['parallel'].items():
                xs.append(int(cores))
                mean = np.mean(np.array(v_['time']).flatten())
                ys.append(mean)
                lower, upper = conf_95_mean(np.array(v_['time']).flatten())
                lowers.append(mean - lower)
                uppers.append(upper - mean)
            ys = [y for _, y in sorted(zip(xs, ys))]
            xs = sorted(xs)
            plt.figure()
            plt.errorbar(xs, ys, yerr=[lowers, uppers], fmt='o',
                         markerfacecolor='none')
            plt.title('{}.cnf'.format(f))
            plt.ylim(0, max(ys) + 500)
            plt.xlabel('# cores')
            plt.ylabel('avg runtime [ms]')
            plt.savefig('figures/{}.png'.format(f))

    def _speedup_plot(self):
        for f, v in self.data.items():
            xs = []
            ys = []
            lowers = []
            uppers = []
            if 'seq' in v:
                sequential = np.array(v['seq']['time']).flatten()
                for cores, v_ in v['parallel'].items():
                    xs.append(int(cores))
                    parallel = []
                    for times in v_['time']:
                        parallel.append(np.mean(times))
                    speedup = sequential / parallel
                    mean_speedup = np.mean(speedup)
                    ys.append(mean_speedup)
                    lower, upper = conf_95_mean(speedup)
                    lowers.append(mean_speedup - lower)
                    uppers.append(upper - mean_speedup)
                ys = [y for _, y in sorted(zip(xs, ys))]
                xs = sorted(xs)
                plt.figure()
                plt.errorbar(xs, ys, yerr=[lowers, uppers], fmt='o',
                             markerfacecolor='none')
                plt.plot([0, max(xs)], [0, max(xs)], '--')
                plt.title('{}.cnf'.format(f))
                plt.xlabel('# cores')
                plt.ylabel('speedup')
                plt.savefig('figures/{}_speedup.png'.format(f))


@click.command()
@click.option('--rerun/--no-rerun', default=False,
              help='Rerun the experiment')
@click.option('--process/--no-process', default=False,
              help='Process experiment')
def main(rerun, process):
    e = DpllScaling()
    if rerun:
        e.run_experiment()
    if process:
        e.process_euler_experiment()
        e.results_to_json()
    else:
        e.results_from_json()
        e.plot()


if __name__ == '__main__':
    main()
