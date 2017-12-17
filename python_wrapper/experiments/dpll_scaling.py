import os
import sys

import click
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import spline

from abstract_experiment import AbstractExperiment
from utils import get_netz_username, parse_into_dict, conf_95_mean

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from euler import EulerTester

NAME_MAP = {'parallel': 'Parallel', 'stealing': 'Work Stealing'}


class DpllScaling(AbstractExperiment):
    def __init__(self):
        super(DpllScaling, self).__init__()
        self.name = 'DpllScaling'

    def run_experiment(self):
        test_folder = 'benchmark_formulas'
        # number of cores
        num_nodes = [2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 36, 40, 44, 48]
        # number of runs per formula
        num_runs = 10
        # timeout per formula in seconds
        timeout = 120
        # overall runtime in minutes
        overall_runtime_minutes = 1400
        nethz_username = get_netz_username()
        tester = EulerTester(test_folder, nethz_username, num_nodes, num_runs,
                             timeout, overall_runtime_minutes)
        tester.run_test(basepath='..', script='bsub_script_scaling.sh')

    def process_euler_experiment(self, tar):
        self.data = parse_into_dict(tar)

    def plot(self):
        for p in ['parallel', 'stealing']:
            self._runtime_plot(p)
            self._speedup_plot(p)
        files = ['uf50-01', 'par8-1-c', 'ais6', 'flat75-4', 'par8-4-', 'anomaly']
        colors = ['teal', 'firebrick', 'darkorange', 'royalblue',
                  'palevioletred', 'slateblue']
        offsets = [-0.25, 0.25, 0, 0.125, -0.125,
                   -0.25, 0.25, 0, 0.125, -0.125,
                   -0.25, 0.25, 0, 0.125, -0.125]
        markers = ['o', 'd', 's', '^', 'x', 'D']
        figure = plt.figure()
        ax = figure.add_subplot(1, 1, 1)
        for f in range(len(files)):
            self._plot_runtime(self.data[files[f]], 'parallel', ax,
                               color=colors[f], offset=offsets[f])
        #plt.show()
        for p in ['parallel', 'stealing']:
            figure = plt.figure()
            ax = figure.add_subplot(1, 1, 1)
            for f in range(len(files)):
                self._plot_speedup(self.data[files[f]], p, ax,
                                   color=colors[f], marker=markers[f],
                                   offset=offsets[f], plotline=(f == 0),
                                   legend=files[f])
            plt.title('Speedup with {} Version'.format(NAME_MAP[p]))
            plt.xlabel('# cores')
            plt.ylabel('speedup')
            handles, labels = ax.get_legend_handles_labels()
            h = handles.pop(0)
            handles.append(h)
            l = labels.pop(0)
            labels.append(l)
            plt.legend(handles, labels)
            plt.tight_layout(pad=0)
            plt.savefig('../../report/figures/dpll_scaling_{}.pdf'.format(p),
                        format='pdf')
            #plt.show()
            figure = plt.figure()
            ax = figure.add_subplot(1, 1, 1)
            for f in range(len(files)):
                self._plot_waiting(self.data[files[f]], p, ax,
                                   color=colors[f], marker=markers[f],
                                   offset=offsets[f], label=files[f])
            plt.title('Overall waiting time per cnf {} Version'.format(
                NAME_MAP[p]))
            plt.xlabel('# cores')
            plt.ylabel('overall waiting time [ms]')
            plt.legend()
            plt.tight_layout(pad=0)
            plt.savefig('../../report/figures/dpll_waiting_{}.pdf'.format(p),
                        format='pdf')
            #plt.show()

    def _runtime_plot(self, parallel_key):
        for f, v in self.data.items():
            figure = plt.figure().add_subplot(1, 1, 1)
            self._plot_runtime(v, parallel_key, figure)
            plt.title('{}.cnf'.format(f))
            plt.xlabel('# cores')
            plt.ylabel('avg total waiting time [ms]')
            plt.savefig('figures/{}_{}.png'.format(f, parallel_key))

    def _plot_runtime(self, data, parallel_key, figure, color='teal', offset=0.0):
        if parallel_key not in data:
            return
        xs, ys, lowers, uppers = [], [], [], []
        if 'seq' in data:
            xs.append(1 + offset)
            mean = np.mean(np.array(data['seq']['time']).flatten())
            ys.append(mean)
            lower, upper = conf_95_mean(np.array(data['seq']['time']).flatten())
            lowers.append(mean - lower)
            uppers.append(upper - mean)
        for cores, v_ in data[parallel_key].items():
            xs.append(int(cores))
            mean = np.mean(np.array(v_['time']).flatten())
            ys.append(mean)
            lower, upper = conf_95_mean(np.array(v_['time']).flatten())
            lowers.append(mean - lower)
            uppers.append(upper - mean)
        ys = [y for _, y in sorted(zip(xs, ys))]
        xs = sorted(xs)
        figure.errorbar(xs, ys, yerr=[lowers, uppers], fmt='o',
                        markerfacecolor='none', color=color)
        xinter = np.linspace(min(xs), max(xs), 300)
        yinter = spline(xs, ys, xinter)
        plt.plot(xinter, yinter, '-', alpha=0.5, linewidth=0.5,
                 color=color)

    def _speedup_plot(self, parallel_key):
        for f, v in self.data.items():
            figure = plt.figure().add_subplot(1, 1, 1)
            self._plot_speedup(v, parallel_key, figure)
            plt.title('{}.cnf'.format(f))
            plt.xlabel('# cores')
            plt.ylabel('speedup')
            plt.savefig('figures/{}_speedup_{}.png'.format(f, parallel_key))

    def _plot_speedup(self, data, parallel_key, figure, color='teal',
                      marker='o', offset=0.0, plotline=True, legend=''):
        if parallel_key not in data:
            return
        xs, ys, lowers, uppers = [], [], [], []
        if 'seq' in data:
            sequential = np.array(data['seq']['time']).flatten()
            for cores, v_ in data[parallel_key].items():
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
            xs = list(map(lambda x: float(x) + offset, xs))
            if legend == '':
                figure.errorbar(xs, ys, yerr=[lowers, uppers], fmt=marker,
                                markerfacecolor='none', color=color)
            else:
                figure.errorbar(xs, ys, yerr=[lowers, uppers], fmt=marker,
                                markerfacecolor='none', color=color,
                                label=legend)
            #xinter = np.linspace(min(xs), max(xs), 300)
            #yinter = spline(xs, ys, xinter)
            #plt.plot(xinter, yinter, '-', alpha=0.5, linewidth=0.5,
            #         color=color)
            plt.plot(xs, ys, '-', alpha=0.5, linewidth=0.5, color=color)
            if plotline:
                plt.plot([0, max(xs)], [0, max(xs)], '-', color='black',
                         label='Linear Speedup')

    def _plot_waiting(self, data, parallel_key, figure, color='teal',
                      offset=0.0, label='', marker='o'):
        if parallel_key not in data:
            return
        xs, ys, lowers, uppers = [], [], [], []
        for cores, v_ in data[parallel_key].items():
            xs.append(int(cores) + offset)
            sums = list(map(lambda wt: np.sum(np.array(wt[1:])),
                             v_['wait']))
            mean = np.mean(sums)
            ys.append(mean)
            lower, upper = conf_95_mean(sums)
            lowers.append(mean - lower)
            uppers.append(upper - mean)
        ys = [y for _, y in sorted(zip(xs, ys))]
        lowers = [l for _, l in sorted(zip(xs, lowers))]
        uppers = [u for _, u in sorted(zip(xs, uppers))]
        xs = sorted(xs)
        figure.errorbar(xs, ys, yerr=[lowers, uppers], fmt=marker,
                        markerfacecolor='none', color=color, label=label)
        plt.plot(xs, ys, '-', alpha=0.5, linewidth=0.5, color=color)

@click.command()
@click.option('--rerun/--no-rerun', default=False,
              help='Rerun the experiment')
@click.option('--process/--no-process', default=False,
              help='Process experiment')
@click.option('--tar', default='measurement.tar',
              help='Tarfile to process (default: measurement.tar')
def main(rerun, process, tar):
    e = DpllScaling()
    if rerun:
        e.run_experiment()
    elif process:
        e.process_euler_experiment(tar)
        e.results_to_json()
    else:
        e.results_from_json()
        e.plot()


if __name__ == '__main__':
    main()
