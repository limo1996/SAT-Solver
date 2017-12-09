import os

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# creates table for communication
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

        self.data['wait']['stealing'] = self.process_case('stealing.wait')
        self.data['wait']['parallel'] = self.process_case('parallel.wait')
        self.data['comm']['stealing'] = self.process_case('stealing.comm')
        self.data['comm']['parallel'] = self.process_case('parallel.comm')

    def process_case(self, case):
        result = {}
        for f in self.getfiles(case):
            print f
            file = open(f, 'r')
            res = {}
            counts = {}
            sums = {}
            for line in file:
                line = line.strip()
                line = line.strip('\x00')
                line = line.replace('\x00', '')
                line = line.replace('  ', ' ')
                if not line or line == "":
                    continue
                numbers = map(int, line.split(' '))
                if len(numbers) in counts:
                    counts[len(numbers)] += 1
                    sums[len(numbers)] += sum(numbers)
                else:
                    counts[len(numbers)] = 1
                    sums[len(numbers)] = sum(numbers)
            x = []
            y = []
            for i in counts:
                x.append(i)
                y.append(sums[i] / counts[i])
            res['x'] = x
            res['y'] = y
            result[f] = res
        return result

    def plot(self):
        self.plot_case(self.data['wait'], 'waiting time')
        self.plot_case(self.data['comm'], '# communication')
        
    def plot_case(self, case, ylabel):
        colors = {};
        for key, value in case['parallel'].iteritems():
            #plt.plot(value['x'], value['y'], 'r')
            print key 
            print value['x']
            print value['y']

        for key, value in case['stealing'].iteritems():
            #plt.plot(value['x'], value['y'], 'b')

            print key 
            print value['x']
            print value['y']

        plt.xlabel('threads')
        plt.ylabel(ylabel);
        plt.legend(handle=[mpatches.Patch(color='r', label='Parallel version'),
                           mpatches.Patch(color='b', label='Stealing version')])
        plt.show()
