import os
import re
import tarfile
import socket
import subprocess
import time
import shutil
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


def parse_into_dict(tar_name):
    tar = tarfile.open(tar_name)
    folder_name = tar_name.split('.tar')[0]
    if os.path.exists(folder_name):
        shutil.rmtree(folder_name)
    tar.extractall(path=folder_name)
    tar.close()
    files = ['{}/{}'.format(folder_name, f)
             for f in os.listdir(folder_name)]
    ret = {}
    sequentials = []
    parallels = {'stealing': [], 'parallel': []}
    for f in files:
        if 'parallel' in f:
            parallels['parallel'].append(f)
        elif 'stealing' in f:
            parallels['stealing'].append(f)
        else:
            sequentials.append(f)
    for seq in sequentials:
        name = _get_seq_cnf_name(seq)
        if name not in ret:
            ret[name] = {}
            ret[name]['seq'] = {'time': []}
        for l in _get_lines(seq):
            ret[name]['seq']['time'].append(_parse_line(l))
    # k is either 'stealing' or 'parallel' and v the array of files
    for k, v in parallels.items():
        for par in v:
            name, measure = _parse_cnf_filename(par, k)
            if name not in ret:
                ret[name] = {}
            if k not in ret[name]:
                ret[name][k] = {}
            for l in _get_lines(par):
                line = _parse_line(l)
                if len(line) not in ret[name][k]:
                    ret[name][k][len(line)] = {}
                if measure not in ret[name][k][len(line)]:
                    ret[name][k][len(line)][measure] = []
                ret[name][k][len(line)][measure].append(_parse_line(l))
    return ret


def _get_seq_cnf_name(filename):
    filename = filename.split('/')[-1]
    return re.match("(\S+).time", filename).group(1)


def _parse_cnf_filename(filename, suffix):
    filename = filename.split('/')[-1]
    matches = re.match("(\S+)_{}.(\S+)".format(suffix), filename)
    return matches.group(1), matches.group(2)


def _get_lines(filename):
    f = open(filename, 'r')
    return [line.replace('\n', '') for line in f if not line == '\n']


def _parse_line(line):
    line = line.rstrip()
    tokens = line.split(' ')
    return [int(token) for token in tokens]


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
    lower, upper = stats.t.interval(0.95,
                            len(measurements) - 1,
                            loc=np.mean(measurements),
                            scale=stats.sem(measurements))
    if lower != lower:  # check if nan
        lower = np.mean(measurements)
    if upper != upper:  # check if nan
        upper = np.mean(measurements)
    return lower, upper


def get_netz_username():
    nethz_username = raw_input('please enter your nethz_account name : ')
    return nethz_username
