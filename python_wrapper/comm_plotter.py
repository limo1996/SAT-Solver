import os
import sys

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

sys.path.append('experiments')

from utils import conf_95_mean

# creates table for communication
class CommPlotter(object):
    def __init__(self, folder):
        self.folder = folder
        self.selected_tests = (
            'flat75-4',
            'par8-1-c'
        )
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

        self.data['wait']['stealing'] = self.process_case('stealing.wait')
        self.data['wait']['parallel'] = self.process_case('parallel.wait')
        self.data['comm']['stealing'] = self.process_case('stealing.comm')
        self.data['comm']['parallel'] = self.process_case('parallel.comm')

    def process_case(self, case):
        result = {}
        for f in self.getfiles(case):
            if not any(str in f for str in self.selected_tests):
                continue
            print f
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
                if len(numbers) in sums:
                    sums[len(numbers)].append(sum(numbers))
                else:
                    sums[len(numbers)] = []
                    sums[len(numbers)].append(sum(numbers))
            x = []
            y = []
            for k, v in sums.iteritems():
                x.append(k)
                if 'wait' in case:
                    y.append(v)
                else:
                    y.append(sum(v) / k)
            res['x'] = x
            res['y'] = y
            result[f] = res
        return result

    def plot(self):
        for test in self.selected_tests:
            self.plot_wait(self.data['wait'], test)
            self.plot_case(self.data['comm'], test, 'bytes transfered')
        
    def plot_case(self, case, test, ylabel):
        for key, value in case['parallel'].iteritems():
            if test in key:
                plt.plot(value['x'], value['y'], 'ro', label='Parallel')
                print key 
                print value['x']
                print value['y']

        for key, value in case['stealing'].iteritems():
            if test in key:
                plt.plot(value['x'], value['y'], 'bo', label='Stealing')
                print key 
                print value['x']
                print value['y']

        plt.title(test)
        plt.xlabel('threads')
        plt.ylabel(ylabel);
        plt.legend(loc=1)
        plt.show()
    
    def plot_wait(self, case, test):
        for key, value in case['parallel'].iteritems():
            if test in key:
                lower_error = []
                upper_error = []
                ys = []
                for data in value['y']:
                    mean = np.mean(data)
                    ys.append(mean)
                    lower, upper = conf_95_mean(data)
                    lower_error.append(mean-lower)
                    upper_error.append(upper-mean)
                plt.errorbar(value['x'], ys,
                    yerr=[lower_error, upper_error],
                    label='Parallel',
                    fmt='o',
                    color='r',
                    markerfacecolor='none')

        for key, value in case['stealing'].iteritems():
            if test in key:
                lower_error = []
                upper_error = []
                ys = []
                for data in value['y']:
                    mean = np.mean(data)
                    ys.append(mean)
                    lower, upper = conf_95_mean(data)
                    lower_error.append(mean-lower)
                    upper_error.append(upper-mean)
                plt.errorbar(value['x'], ys,
                    yerr=[lower_error, upper_error],
                    label='Stealing',
                    fmt='D',
                    color='b',
                    markerfacecolor='none')
        plt.title(test)
        plt.legend(loc=1)
        plt.xlabel('threads')
        plt.ylabel('average waiting time [ms]');
        plt.show()
        '''plt.title(test)
        plt.xlabel('threads')
        plt.ylabel('average waiting time [ms]');

        plt.show()'''
