import os
import re
from sets import Set
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

"""
Draws plots from files located in folder that is passed in constructor
"""
class Plotter(object):
    def __init__(self, folder):
        folder = os.path.join(os.pardir, folder)
        self.folder = folder
        self.files = [os.path.join(folder, f)
                      for f in os.listdir(folder)
                      if os.path.isfile(os.path.join(folder, f))]
        self.parse_files()
        self.plot_grapf()
    
    def parse_files(self):
        print "parsing files..."
        global_dic = []
        threads = Set()
        for f in self.files:
            print(f)
            file = open(f, 'r')
            local_dic = {}
            counts_dic = {}
            for line in file:
                line = line.strip()
                line = line.strip('\x00')
                if not line or line == "":
                    continue
                numbers = map(int, line.split(' '))
                local_len = len(numbers)
                threads.add(local_len)
                sum = float(0)
                for i in range(0, local_len):
                    sum += numbers[i]
                sum = float(sum/local_len)
                if local_len in local_dic:
                    counts_dic[local_len] += 1
                    local_dic[local_len] += float(sum)
                else:
                    local_dic[local_len] = float(sum)
                    counts_dic[local_len] = 1
            for i in threads:
                local_dic[i] = float(local_dic[i]/counts_dic[i])
            pattern = re.compile(r"""3-sat_var_(?P<n1>.*?)_cla_(?P<n2>.*?)_is-sat_case_(?P<ccc>.*?).time""", re.VERBOSE)
            match = pattern.match(os.path.basename(f))
            case = match.group("ccc")
            variables = float(match.group("n1"))
            clauses = float(match.group("n2"))
            global_dic.append((variables, local_dic))
        self.plot_data = global_dic
        self.plot_threads = threads

    def plot_grapf(self):
        print "plotting graph:"
        colors = ['ro', 'bo', 'go', 'co', 'mo', 'yo', 'ko', 'wo']
        patches = []
        self.plot_threads = sorted(self.plot_threads)
        j = 1
        for i in self.plot_threads:
            if i != 1:
                patches.append(mpatches.Patch(color=colors[j][0], label="Parallel version - {i} nodes".format(i=i)))
                j += 1
            else:
                patches.insert(0, mpatches.Patch(color='r', label='Sequential version'))

        var_arr = {}
        vars = []
        for i in self.plot_threads:
            var_arr[i] = []

        for i in self.plot_data:
            vars.append(i[0])
            for j in self.plot_threads:
                var_arr[j].append(i[1][j])
                print"x:{first} y:{second}".format(first=i[0], second=i[1][j])

        j = 1
        for i in self.plot_threads:
            if i == 1:
                plt.plot(vars, var_arr[i], colors[0])
            else:
                plt.plot(vars, var_arr[i], colors[j])
                j += 1

        plt.xlabel("Variables")
        plt.ylabel("Time (ms)")
        plt.legend(handles=patches)
        plt.show()