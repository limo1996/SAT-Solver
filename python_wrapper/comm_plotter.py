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
            'flat75-4', #perfect
            #'par8-1-c', #comm not good
            'ais6', #good
            #'ais8', #quite weird but ok
            #'anomaly',
            #'flat50-1',#good
            #'ii8a1',#weird comm for 32 and 48
            #'ii8a2', parallel failed
            #'medium',#perfect
            #'par8-5'#perfect
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
                print sum(numbers)
                print len(numbers)
                if len(numbers) not in sums:
                    sums[len(numbers)] = []
                if 'parallel.wait' in case:
                    sums[len(numbers)].append(np.mean(numbers[1:]))
                elif 'wait' in case:
                    sums[len(numbers)].append(np.mean(numbers))
                else:
                    sums[len(numbers)].append(sum(numbers))
            x = []
            y = []
            for k, v in sums.iteritems():
                x.append(k)
                if 'wait' in case:
                    y.append(v)
                else:
                    y.append(sum(v) / len(v))
            res['x'] = x
            res['y'] = y
            print x
            print y
            result[f] = res
        return result

    def plot(self):
        #for test in self.selected_tests:
        self.plot_wait(self.data['wait'], self.selected_tests)
        self.plot_comm(self.data['comm'], self.selected_tests, 'bytes transfered')
        
    def plot_comm(self, case, tests, ylabel):
        i = 0
        colors = ['ro', 'go', 'bo', 'mo']
        for key, value in case['parallel'].iteritems():
            for test in tests:
                if test in key:
                    plt.plot(value['x'], value['y'], colors[i], label='Parallel {}'.format(test))
                    print key 
                    print value['x']
                    print value['y']
                    i+=1

        for key, value in case['stealing'].iteritems():
            for test in tests:
                if test in key:
                    plt.plot(value['x'], value['y'], colors[i], label='Stealing {}'.format(test))
                    print key 
                    print value['x']
                    print value['y']
                    i+=1

        plt.title(test)
        plt.xlabel('threads')
        plt.ylabel(ylabel);
        plt.legend(loc=1)
        plt.gcf().savefig('../results/communication/{}.pdf'.format(test),
                    format='pdf')
        plt.show()
    
    def plot_wait(self, case, tests):
        i = -1
        colors = ['r', 'b', 'g', 'm']
        for key, value in case['parallel'].iteritems():
            for test in tests:
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
                    i+=1
                    plt.errorbar(value['x'], ys,
                        yerr=[lower_error, upper_error],
                        label='Parallel {}'.format(test),
                        fmt='o',
                        color=colors[i],
                        markerfacecolor='none')

        for key, value in case['stealing'].iteritems():
            for test in tests:
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
                    i+=1
                    plt.errorbar(value['x'], ys,
                        yerr=[lower_error, upper_error],
                        label='Stealing {}'.format(test),
                        fmt='D',
                        color=colors[i],
                        markerfacecolor='none')
        plt.title(test)
        plt.legend(loc=1)
        plt.xlabel('threads')
        plt.ylabel('average waiting time [ms]');
        plt.gcf().savefig('../results/waiting/{}.pdf'.format(test),
                    format='pdf')
        plt.show()