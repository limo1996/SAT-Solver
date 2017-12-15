import subprocess
import os

PROGRAM = '../sequential_main -cerr-level 1'
FOLDER = '../cnfs/benchmark_formulas'


def main():
    files = sorted(['../cnfs/benchmark_formulas/{}'.format(f)
                    for f in os.listdir(FOLDER)
                    if os.path.isfile(os.path.join(FOLDER, f))
                    and f.endswith('.cnf')])
    for f in files:
        diff = run(f)
        print('Formula {} needs {} dpll branches'.format(f, diff))


def run(cnf):
    command = PROGRAM + " " + cnf
    p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    p.wait()
    stderr = [line.decode('utf-8').strip() for line in iter(p.stderr.readline, b'')]
    return len(stderr)


if __name__ == '__main__':
    main()
