import os

import math
import click
import matplotlib.pyplot as plt
import numpy as np

from abstract_experiment import AbstractExperiment
from utils import run_n_times, get_results, delete_files_in_folder, get_info, \
    Info, conf_95_mean

FOLDER = 'dpll_vs_cdcl'
parent_parent = os.path.join(os.pardir, os.pardir)
CNF_FOLDER = os.path.join(parent_parent, os.path.join('cnfs', FOLDER))
EXECUTABLE = './sequential_main'
REPETITIONS = 10
TIMEOUT = 16

VAR_LOWER = 5
VAR_UPPER = 42
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
                self.data[s][f]['info'] = get_info(f).__dict__
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
                info = Info(**v['info'])
                if info.variables < VAR_LOWER or info.variables > VAR_UPPER:
                    continue
                mean = np.mean(np.array(v['time'], dtype=float))
                if not math.isnan(mean):
                    if s == 'DPLL':
                        dpll_times[k] = mean
                    else:
                        cdcl_times[k] = mean
        dpll_times_lower1 = []
        cdcl_times_lower1 = []
        for k, v in dpll_times.iteritems():
            if k in cdcl_times:
                d = dpll_times[k]
                c = cdcl_times[k]
                speedup.append(d / c)
                if d/c <= 1:
                    dpll_times_lower1.append(d)
                    cdcl_times_lower1.append(c)
        print('Total number of cnfs in plots: {}'.format(len(dpll_times)))
        print('Number of cnfs in sub 1 plot:  {}'.format(
            len(dpll_times_lower1)))
        f, axes = plt.subplots(2, 1, figsize=(6, 4))
        axes[0].set_title('Speedup of CDCL over DPLL')
        self._violin_plot(axes[0], speedup, np.max(speedup))
        axes[1].set_title('Speedup of CDCL over DPLL (zoomed in)')
        self._violin_plot(axes[1], speedup, 6)
        plt.tight_layout(pad=0, w_pad=0, h_pad=0)
        f.savefig('{}/dpll_vs_cdcl_speedup.pdf'.format(self.figures_folder),
                    format='pdf')
        plt.show()

        f, axes = plt.subplots(2, 1, figsize=(6, 4))
        axes[0].set_title('Histogram of Runtimes')
        self._histo_plot(axes[0], dpll_times.values(), 'firebrick', 'DPLL')
        self._histo_plot(axes[0], cdcl_times.values(), 'teal', 'CDCL')
        self._histo_plot(axes[1], dpll_times_lower1, 'firebrick', 'DPLL')
        self._histo_plot(axes[1], cdcl_times_lower1, 'teal', 'CDCL')
        axes[1].set_title('Histogram of Runtimes for sub 1 Speedup')

        plt.tight_layout(pad=0, w_pad=0, h_pad=0)
        f.savefig('{}/dpll_vs_cdcl_histo.pdf'.format(self.figures_folder),
                    format='pdf')
        plt.show()

    @staticmethod
    def _violin_plot(plot_handle, speedup, ylimit):
        parts = plot_handle.violinplot(speedup,
                                       showextrema=False, showmeans=True,
                                       showmedians=True, widths=0.5)
        for pc in parts['bodies']:
            pc.set_edgecolor('black')
        legend_handles = [parts['cmeans'], parts['cmedians']]
        parts['cmeans'].set_color('seagreen')
        parts['cmedians'].set_color('darkorange')
        plot_handle.set_ylim([0, ylimit])
        h, = plot_handle.plot([0.3, 1.8], [1, 1], '--', color='firebrick')
        legend_handles.append(h)
        plot_handle.set_xticks([])
        plot_handle.set_ylabel('Speedup')
        plot_handle.set_xlabel('Distribution')
        plot_handle.legend(legend_handles, ['Mean', 'Median', 'Speedup 1'])

    @staticmethod
    def _histo_plot(plot_handle, times, color, label):
        plot_handle.hist(times,
                         np.arange(HISTO_LOWER, HISTO_UPPER, HISTO_STEP_SIZE),
                         color=color, alpha=0.6, label=label)
        plot_handle.set_ylabel('Frequency [count]')
        plot_handle.set_xlabel('Runtime [ms]')
        plot_handle.legend()

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
                info = Info(**v['info'])
                data = []
                for t in v['time']:
                    data.append(t)
                if info.variables not in x_offsets:
                    x_offsets[info.variables] = -1
                x_offsets[info.variables] = x_offsets[info.variables] + 0.1
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
