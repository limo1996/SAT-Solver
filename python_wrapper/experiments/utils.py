import os
import re
import socket
import subprocess
import time
from collections import namedtuple

import numpy as np
import scipy.stats as stats

Info = namedtuple('Info', ['variables', 'clauses', 'is_sat', 'case'])


def get_experiment_id():
    machine = socket.gethostname()
    t = time.strftime('%Y-%m-%d_%H-%M')
    return machine + '--' + t


def get_info(filename):
    filename = filename.split("/")[-1]
    pattern = re.compile(
        r"""3-sat_var_(?P<n1>.*?)_cla_(?P<n2>.*?)_is-(?P<sat>.*?)_case_(?P<ccc>.*?).cnf""",
        re.VERBOSE)
    match = pattern.match(os.path.basename(filename))
    variables = int(match.group("n1"))
    clauses = int(match.group("n2"))
    sat = str(match.group("sat")) == 'sat'
    case = int(match.group("ccc"))
    return Info(variables=variables, clauses=clauses, is_sat=sat, case=case)


def run_n_times(command, n):
    for i in range(0, n):
        p = subprocess.Popen(command, shell=True,
                             stdout=subprocess.PIPE,
                             stderr=None)
        p.wait()


def get_results(filename):
    if not os.path.exists(filename):
        return []
    f = open(filename, 'r')
    data = []
    for line in f:
        line = line.strip()
        line = line.strip('\x00')
        line = line.replace('\x00', '')
        if not line or line == "":
            continue
        numbers = map(int, line.split(' '))
        data.append(numbers)
    return data


def delete_files_in_folder(folder, file_extension):
    subprocess.call('rm -f {}/*.{}'.format(folder, file_extension), shell=True)


def conf_95_mean(measurements):
    return stats.t.interval(0.95,
                            len(measurements) - 1,
                            loc=np.mean(measurements),
                            scale=stats.sem(measurements))
