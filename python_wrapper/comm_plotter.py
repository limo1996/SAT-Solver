import os
import sys

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

from scipy.interpolate import spline

sys.path.append('experiments')
from utils import conf_95_mean
# plots communication and waiting time comparisons between stealing and parallel
class CommPlotter(object):
    def __init__(self, folder):
        self.folder = folder
        self.process()
        self.plot()

    def getfiles(self, extension):
        return [os.path.join(self.folder, f)
            for f in os.listdir(self.folder)
            if os.path.isfile(os.path.join(self.folder, f)) and f.endswith(extension)] 

    def process(self):
        self.data = {}
        self.data['wait'] = {}
        self.data['comm'] = {}
        self.data['comm2'] = {}

        self.data['wait']['stealing'] = self.process_case('stealing.wait')
        self.data['wait']['parallel'] = self.process_case('parallel.wait')
        self.data['comm']['stealing'] = self.process_case('stealing.comm')
        self.data['comm']['parallel'] = self.process_case('parallel.comm')
        self.data['comm2']['stealing'] = self.process_case('stealing.comm2')
        self.data['comm2']['parallel'] = self.process_case('parallel.comm2')

    def process_case(self, case):
        result = {}
        tmpData = {}
        for f in self.getfiles(case):
            file = open(f, 'r')
            res = {}
            sums = {}
            for line in file:
                line = line.strip()
                line = line.strip('\x00')
                line = line.replace('\x00', '')
                line = line.replace('  ', ' ')
                if not line or line == "":
                    continue
                numbers = map(int, line.split(' '))
                if len(numbers) not in sums:
                    sums[len(numbers)] = []
                if 'parallel.wait' in case:
                    sums[len(numbers)].append(np.mean(numbers[1:]))
                elif 'wait' in case:
                    sums[len(numbers)].append(np.mean(numbers))
                else:
                    sums[len(numbers)].append(sum(numbers))
            for k, v in sums.iteritems():
                if k not in tmpData:
                    tmpData[k] = []
                tmpData[k].append(sum(v) / len(v))
        x = []
        y = []
        for k, v in sorted(tmpData.iteritems()):
            x.append(k)
            y.append(v)
        result['x'] = x
        result['y'] = y
        return result

    def plot(self):
        self.plot_case(self.data['wait'], 'overall waiting time [ms]', 'Waiting')
        self.plot_case(self.data['comm'], 'bytes transfered [B]', 'Communication')
        self.plot_case(self.data['comm2'], '# of meta data sent', 'Requests')
        
    def plot_case(self, case, ylabel, title):
        i = 0
        colors = {}
        colors['stealing'] = ['firebricks', 'firebrick']
        colors['parallel'] = ['teal', 'teal']
        offset = {}
        offset['parallel'] = -0.6
        offset['stealing'] = 0

        for type in ['parallel', 'stealing']:
            lower_error = []
            upper_error = []
            ys = []
            xs = []
            for data in case[type]['x']:
                xs.append(data + offset[type])
            for data in case[type]['y']:
                mean = np.mean(data)
                ys.append(mean)
                lower, upper = conf_95_mean(data)
                lower_error.append(mean-lower)
                upper_error.append(upper-mean)
            plt.errorbar(xs, ys,
                    yerr=[lower_error, upper_error],
                    #label='{} version'.format(type),
                    fmt='D',
                    color=colors[type][1],
                    linewidth=1,
                    markerfacecolor='none')
            
            plt.plot(xs, ys, '--', color=colors[type][1], label='{} version'.format(type), linewidth=1)

        plt.title(title)
        plt.xlabel('# cores')
        plt.ylabel(ylabel);
        plt.legend(loc=1)
        plt.gcf().savefig('../results/communication/overall.pdf', format='pdf')
        plt.show()