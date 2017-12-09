import os
import sys

import click
import numpy as np
import matplotlib.pyplot as plt

from abstract_experiment import AbstractExperiment
from utils import get_netz_username, parse_into_dict

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
        num_runs = 10
        # timeout per formula in seconds
        timeout = 10
        # overall runtime in minutes
        overall_runtime_minutes = 500
        nethz_username = get_netz_username()
        tester = EulerTester(test_folder, nethz_username, num_nodes, num_runs,
                             timeout, overall_runtime_minutes)
        tester.run_test(basepath='..')

    def process_euler_experiment(self):
        self.data = parse_into_dict('measurements.tar')

    def plot(self):
        for f, v in self.data.items():
            xs = []
            ys = []
            xs.append(1)
            ys.append(np.mean(np.array(v['seq']['time']).flatten()))
            for cores, v_ in v['parallel'].items():
                xs.append(cores)
                ys.append(np.mean(np.array(v_['time']).flatten()))
            plt.plot(xs, ys, 'o')
            plt.title(f)
            plt.ylim(0, max(ys) + 500)
            plt.xlabel('# cores')
            plt.ylabel('avg runtime [ms]')
            plt.show()


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
    e.results_from_json()
    e.plot()


if __name__ == '__main__':
    main()
