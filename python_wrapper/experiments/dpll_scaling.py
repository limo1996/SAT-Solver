import os
import sys

import click
import matplotlib.pyplot as plt
import numpy as np

from abstract_experiment import AbstractExperiment
from utils import get_netz_username, parse_into_dict, conf_95_mean

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from euler import EulerTester

NAME_MAP = {'parallel': 'Master Worker', 'stealing': 'Work Stealing'}
NAME_POST_FIX = 'Model'
COLORS = ['teal', 'firebrick', 'darkorange', 'royalblue',
          'palevioletred', 'slateblue', 'black', 'grey']
OFFSETS = [-0.25, 0.25, 0, 0.125, -0.125,
           0.25, -0.25, 0, 0.125, -0.125,
           -0.25, 0.25, 0, 0.125, -0.125]
MARKERS = ['o', 'd', 's', '^', 'x', 'D', 'v', '.']

MEW = 1.75
MS = 6
FIG_WIDTH = 6
FIG_HEIGHT = 3.4
FIG_HEIGHT_SMALL = 2.5


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
        self.data['tar'] = tar

    def plot(self):
        subset = ['uf50-01', 'par8-1-c', 'ais6', 'flat75-4', 'anomaly',
                  'uuf50-01']

        all_files = self.data.keys()
        all_files.remove('tar')
        non_subset = list(set(all_files) - set(subset))

        #self.runtime_plots(subset, 'subset')
        #self.runtime_plots(non_subset, 'non_subset')
        self.speedup_plots(subset, 'subset')
        self.speedup_plots(non_subset, 'non_subset')
        self.waiting_plots(subset, 'subset')
        self.waiting_plots(non_subset, 'non_subset')
        self.comm_plots(subset, 'subset')
        self.comm_plots(non_subset, 'non_subset')
        self.find_best_runtime(all_files)

    def _default_plt_postprocessing(self):
        plt.tight_layout(pad=0)
        plt.grid('grid', color='whitesmoke', linestyle='-')


    def runtime_plots(self, set, key):
        tar = self.data['tar'].replace('.', '_')
        for p in ['parallel', 'stealing']:
            figure = plt.figure(figsize=(FIG_WIDTH, FIG_HEIGHT))
            ax = figure.add_subplot(1, 1, 1)
            for f in range(len(set)):
                self._plot_runtime(self.data[set[f]], p, ax,
                                   color=COLORS[f], offset=OFFSETS[f],
                                   marker=MARKERS[f], legend=set[f])
            plt.title('Runtime - {} {}'.format(NAME_MAP[p], NAME_POST_FIX))
            plt.xlabel('# cores')
            plt.ylabel('Avg. runtime [ms]')
            plt.legend()
            plt.tight_layout(pad=0)
            f = '../../report/figures/runtime_{}_{}_{}.pdf'.format(p, key, tar)
            plt.savefig(f, format='pdf')
            f = '../../report/figures/runtime_{}_{}_{}.png'.format(p, key, tar)
            plt.savefig(f, format='png')

    def find_best_runtime(self, files):
        for f in range(len(files)):
            data = self.data[files[f]]
            best_config = 'none'
            m = 1000000000
            for p in ['parallel', 'stealing']:
                if p in data:
                    for n, d in data[p].items():
                        times = d['time']
                        means = list(map(lambda t: np.mean(t), times))
                        mean = np.mean(means)
                        lower, upper = conf_95_mean(means)
                        lower = mean - lower
                        upper = upper - mean
                        if mean < m:
                            m = mean
                            best_config = '{} on {} nodes, time: {} -{} +{}'.format(p, n, mean, lower, upper)
            print('formula {} best: {}'.format(files[f], best_config))

    def speedup_plots(self, set, key):
        tar = self.data['tar'].replace('.', '_')
        for p in ['parallel', 'stealing']:
            figure = plt.figure(figsize=(FIG_WIDTH, FIG_HEIGHT))
            ax = figure.add_subplot(1, 1, 1)
            for f in range(len(set)):
                self._plot_speedup(self.data[set[f]], p, ax,
                                   color=COLORS[f], marker=MARKERS[f],
                                   offset=OFFSETS[f], plotline=(f == 0),
                                   legend=set[f])
            plt.title('Speedup - {} {}'.format(NAME_MAP[p], NAME_POST_FIX))
            plt.xlabel('# cores')
            plt.ylabel('Avg. speedup\nwith 95% conf. intervals')
            handles, labels = ax.get_legend_handles_labels()

            if p == 'parallel':
                handles = [handles[0], handles[-1]] + [handles[1]] + \
                    handles[2:-1]
                labels = [labels[0], labels[-1]] + [labels[1]] + labels[2:-1]
            h = handles.pop(0)
            handles.append(h)
            l = labels.pop(0)
            labels.append(l)
            plt.legend(handles, labels)
            self._default_plt_postprocessing()
            f = '../../report/figures/scaling_{}_{}_{}.pdf'.format(p, key, tar)
            plt.savefig(f, format='pdf')
            f = '../../report/figures/scaling_{}_{}_{}.png'.format(p, key, tar)
            plt.savefig(f, format='png')

    def waiting_plots(self, set, key):
        tar = self.data['tar'].replace('.', '_')
        fig_height = FIG_HEIGHT_SMALL
        if key == 'non_subset':
            fig_height = FIG_HEIGHT
        for p in ['parallel', 'stealing']:
            figure = plt.figure(figsize=(FIG_WIDTH, fig_height))
            ax = figure.add_subplot(1, 1, 1)
            for f in range(len(set)):
                self._plot_waiting(self.data[set[f]], p, ax,
                                   color=COLORS[f], marker=MARKERS[f],
                                   offset=OFFSETS[f], label=set[f])
            plt.title('Overall Waiting Time per Formula - {} {}'.format(
                NAME_MAP[p], NAME_POST_FIX))
            plt.xlabel('# cores')
            plt.ylabel('Avg. overall waiting time [s]\nwith 95% conf. '
                       'intervals')
            handles, labels = ax.get_legend_handles_labels()
            if p == 'parallel':
                handles = [handles[-1]] + [handles[0]] + handles[1:-1]
                labels = [labels[-1]] + [labels[0]] + labels[1:-1]
            plt.legend(handles, labels)
            self._default_plt_postprocessing()
            f = '../../report/figures/waiting_{}_{}_{}.pdf'.format(p, key, tar)
            plt.savefig(f, format='pdf')
            f = '../../report/figures/waiting_{}_{}_{}.png'.format(p, key, tar)
            plt.savefig(f, format='png')

    def comm_plots(self, set, key):
        tar = self.data['tar'].replace('.', '_')
        figure = plt.figure(figsize=(FIG_WIDTH, 6))
        axs = {'parallel': figure.add_subplot(2, 1, 1),
               'stealing': figure.add_subplot(2, 1, 2)}
        for p in ['parallel', 'stealing']:
            ax = axs[p]
            for f in range(len(set)):
                self._plot_comm(self.data[set[f]], p, ax,
                                color=COLORS[f], marker=MARKERS[f],
                                offset=OFFSETS[f], label=set[f])
            ax.set_title('Communication - {} {}'.format(
                NAME_MAP[p], NAME_POST_FIX))
            ax.set_xlabel('# cores')
            ax.set_ylabel('Avg. communication [bytes]\nwith 95% conf. '
                          'intervals')
            handles, labels = ax.get_legend_handles_labels()
            if p == 'parallel':
                handles = [handles[-1]] + [handles[0]] + handles[1:-1]
                labels = [labels[-1]] + [labels[0]] + labels[1:-1]
            if p == 'parallel':
                ax.set_ylim(-2500, 100000)
            else:
                ax.set_ylim(-100, 8000)
            yticks = ax.get_yticks()[1:]
            ytick_labels = list(map(lambda t: str(t/1000)[:-2] + ' k', yticks))
            ax.set_yticks(yticks)
            ax.set_yticklabels(ytick_labels)
            if p == 'parallel':
                ax.set_ylim(-2500, 100000)
            else:
                ax.set_ylim(-100, 8000)
            ax.legend(handles, labels)
            ax.grid('grid', color='whitesmoke', linestyle='-')
        plt.tight_layout(pad=0, h_pad=1)
        f = '../../report/figures/comm_{}_{}.pdf'.format(key, tar)
        plt.savefig(f, format='pdf')
        f = '../../report/figures/comm_{}_{}.png'.format(key, tar)
        plt.savefig(f, format='png')

    def _single_formula_runtime_plot(self, parallel_key):
        for f, v in self.data.items():
            if f == 'tar':
                continue
            figure = plt.figure().add_subplot(1, 1, 1)
            self._plot_runtime(v, parallel_key, figure)
            plt.title('{}.cnf'.format(f))
            plt.xlabel('# cores')
            plt.ylabel('Avg total waiting time [ms]')
            plt.savefig('figures/{}_{}.png'.format(f, parallel_key))

    def _plot_runtime(self, data, parallel_key, figure, color='teal',
                      offset=0.0, marker='o', legend=''):
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
            print('{} sequential runtime: {} +{} -{}'.format(legend, mean,
                                                             upper - mean,
                                                             mean - lower))
        for cores, v_ in data[parallel_key].items():
            xs.append(int(cores))
            mean = np.mean(np.array(v_['time']).flatten())
            ys.append(mean)
            lower, upper = conf_95_mean(np.array(v_['time']).flatten())
            lowers.append(mean - lower)
            uppers.append(upper - mean)
        ys = [y for _, y in sorted(zip(xs, ys))]
        xs = sorted(xs)
        figure.errorbar(xs, ys, yerr=[lowers, uppers], fmt=marker,
                        markerfacecolor='none', color=color, label=legend,
                        mew=MEW, ms=MS)
        plt.plot(xs, ys, '-', alpha=0.5, linewidth=0.5, color=color)

    def _single_formula_speedup_plot(self, parallel_key):
        for f, v in self.data.items():
            if f == 'tar':
                continue
            figure = plt.figure().add_subplot(1, 1, 1)
            self._plot_speedup(v, parallel_key, figure)
            plt.title('{}.cnf'.format(f))
            plt.xlabel('# cores')
            plt.ylabel('Speedup')
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
                                markerfacecolor='none', color=color,
                                mew=MEW, ms=MS)
            else:
                figure.errorbar(xs, ys, yerr=[lowers, uppers], fmt=marker,
                                markerfacecolor='none', color=color,
                                label=legend,
                                mew=MEW, ms=MS)
            plt.plot(xs, ys, '-', alpha=0.5, linewidth=0.5, color=color)
            if plotline:
                plt.plot([0, max(xs)], [0, max(xs)], '-', color='black',
                         label='Linear Speedup')
        else:
            print('could not solve {} sequentially within '
                  'timeout...'.format(legend))

    def _plot_waiting(self, data, parallel_key, figure, color='teal',
                      offset=0.0, label='', marker='o'):
        if parallel_key not in data:
            return
        xs, ys, lowers, uppers = [], [], [], []
        for cores, v_ in data[parallel_key].items():
            xs.append(int(cores) + offset)
            sums = list(map(lambda wt: np.sum(np.array(wt[1:])),
                             v_['wait']))
            sums = np.array(sums) / 1000
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
                        markerfacecolor='none', color=color, label=label,
                        mew=MEW, ms=MS)
        plt.plot(xs, ys, '-', alpha=0.5, linewidth=0.5, color=color)

    def _plot_comm(self, data, parallel_key, figure, color='teal',
                   offset=0.0, label='', marker='o'):
        if parallel_key not in data:
            return
        xs, ys, lowers, uppers = [], [], [], []
        for cores, v_ in data[parallel_key].items():
            xs.append(int(cores) + offset)
            sums = list(map(lambda wt: np.sum(np.array(wt[1:])),
                            v_['comm']))
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
                        markerfacecolor='none', color=color, label=label,
                        mew=MEW, ms=MS)
        figure.plot(xs, ys, '-', alpha=0.5, linewidth=0.5, color=color)


@click.command()
@click.option('--rerun/--no-rerun', default=False,
              help='Rerun the experiment')
@click.option('--process/--no-process', default=False,
              help='Process experiment')
@click.option('--tar', default='measurements.tar',
              help='Tarfile to process (default: measurements.tar')
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
